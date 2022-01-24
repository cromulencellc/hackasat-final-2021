/* 
** Purpose: Manage Packet Table that defines which packets will be sent from the
**          software bus to a socket.
**
** Notes:
**   1. This has some of the features of a flight app such as packet filtering but it
**      would need design/code reviews to transition it to a flight mission. For starters
**      it uses UDP sockets and it doesn't regulate output bit rates. 
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Include Files:
*/

#include <errno.h>
#include <string.h>
#include <unistd.h>

#include "app_cfg.h"
#include "sbnmgr.h"

/*
** Global File Data
*/

static SBNMGR_Class*  SbnMgr = NULL;

/*
** Local Function Prototypes
*/

static void  DestructorCallback(void);
static void  FlushTlmPipe(void);
static int32 SubscribeNewPkt(SBNTBL_Pkt* NewPkt);
static void  ComputeStats(uint16 PktsSent, uint32 BytesSent);


/******************************************************************************
** Function: SBNMGR_Constructor
**
*/
void SBNMGR_Constructor(SBNMGR_Class*  SbnMgrPtr, char* PipeName, uint16 PipeDepth)
{

   SbnMgr = SbnMgrPtr;

   // Initialize Tx Socket - Pull into function
   SbnMgr->OutputEnable = TRUE;
   SbnMgr->SuppressSend = FALSE;
   SbnMgr->TxSockId    = 0;
   strncpy(SbnMgr->TxDestIp, SBN_LITE_TX_PEER_ADDR, SBNMGR_IP_STR_LEN);
   CFE_EVS_SendEvent(SBNMGR_DEBUG_EID, CFE_EVS_INFORMATION,
                     "SBN-LITE Startup. TX PEER IP: %s TX PEER PORT: %d, RECV PORT: %d", SBN_LITE_TX_PEER_ADDR, SBN_LITE_TX_PORT, SBN_LITE_RX_PORT);
   if ( (SbnMgr->TxSockId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
         
         CFE_EVS_SendEvent(SBNMGR_OUTPUT_ENA_SOCKET_ERR_EID, CFE_EVS_ERROR,
                           "Telemetry output enable socket error. errno %d", errno);
      }
      else {
         
         SBNMGR_InitStats(SBN_LITE_RUN_LOOP_DELAY_MS,SBNMGR_STATS_STARTUP_INIT_MS);
   }

   // Initialize Rx Socket - Pull into function
   SbnMgr->RxBind = FALSE;
   SbnMgr->RxPktCnt    = 0;
   SbnMgr->RxPktErrCnt = 0;
   SbnMgr->RecvMsgPtr = (CFE_SB_MsgPtr_t) &(SbnMgr->RecvBuff[0]);
   CFE_PSP_MemSet(&(SbnMgr->RxSockAddr), 0, sizeof(SbnMgr->RxSockAddr));

   if ( (SbnMgr->RxSockId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) >= 0) {

      SbnMgr->RxSockAddr.sin_family      = AF_INET;
      // Should probably bind to a specific address
      SbnMgr->RxSockAddr.sin_addr.s_addr = htonl(INADDR_ANY);
      SbnMgr->RxSockAddr.sin_port        = htons(SBN_LITE_RX_PORT);

      if ( (bind(SbnMgr->RxSockId, (struct sockaddr *) &(SbnMgr->RxSockAddr), sizeof(SbnMgr->RxSockAddr)) >= 0) ) {

    	   SbnMgr->RxBind = TRUE;
         /* Set the socket to non-blocking. Not available in vxWorks, so conditionally compiled. */
         #ifdef _HAVE_FCNTL_
            fcntl(SbnMgr->RxSockId, F_SETFL, O_NONBLOCK);
         #endif

         CFE_EVS_SendEvent(SBNMGR_DEBUG_EID,CFE_EVS_INFORMATION,
                           "Sbn-lite Rx Socket bind success. Port: %u", 4322);

      }/* End if successful bind */
      else {
         
         CFE_EVS_SendEvent(SBNMGR_RXSOCKET_BIND_ERR_EID,CFE_EVS_ERROR,
                           "Rx Socket bind failed. Status = %d", errno);
      }

   } /* End if successful socket creation */
   else {
      
      CFE_EVS_SendEvent(SBNMGR_RXSOCKET_CREATE_ERR_EID, CFE_EVS_ERROR, "Rx Socket creation failed. Status = %d", errno);
   
   }

   SBNTBL_SetTblToUnused(&(SbnMgr->Tbl));

   CFE_SB_CreatePipe(&(SbnMgr->TxPipe), PipeDepth, PipeName);
      
   CFE_SB_InitMsg(&(SbnMgr->PktTlm), SBN_LITE_PKT_TBL_TLM_MID, SBNMGR_PKT_TLM_LEN, TRUE);
   
   OS_TaskInstallDeleteHandler((void *)(&DestructorCallback)); /* Called when application terminates */

} /* End SBNMGR_Constructor() */


/******************************************************************************
** Function: SBNMGR_GetTblPtr
**
*/
const SBNTBL_Tbl* SBNMGR_GetTblPtr()
{

   return &(SbnMgr->Tbl);

} /* End SBNMGR_GetTblPtr() */


/******************************************************************************
** Function:  SBNMGR_ResetStatus
**
*/
void SBNMGR_ResetStatus(void)
{

   SBNMGR_InitStats(0,SBNMGR_STATS_RECONFIG_INIT_MS);
   SbnMgr->RxPktCnt = 0;
   SbnMgr->RxPktErrCnt = 0;

} /* End SBNMGR_ResetStatus() */


/******************************************************************************
** Function:  SBNMGR_InitStats
**
** If TxOutputInterval==0 then retain current stats
** ComputeStats() logic assumes at least 1 init cycle
**
*/
void SBNMGR_InitStats(uint16 TxOutputInterval, uint16 InitDelay)
{
   
   if (TxOutputInterval != 0) SbnMgr->Stats.TxOutputInterval = (double)TxOutputInterval;

   SbnMgr->Stats.State = SBNMGR_STATS_INIT_CYCLE;
   SbnMgr->Stats.InitCycles = (SbnMgr->Stats.TxOutputInterval >= InitDelay) ? 1 : (double)InitDelay/SbnMgr->Stats.TxOutputInterval;
            
   SbnMgr->Stats.IntervalMilliSecs = 0.0;
   SbnMgr->Stats.TxIntervalPkts = 0;
   SbnMgr->Stats.TxIntervalBytes = 0;
      
   SbnMgr->Stats.TxPrevIntervalAvgPkts  = 0.0;
   SbnMgr->Stats.TxPrevIntervalAvgBytes = 0.0;
   
   SbnMgr->Stats.TxAvgPktsPerSec  = 0.0;
   SbnMgr->Stats.TxAvgBytesPerSec = 0.0;

} /* End SBNMGR_InitStats() */


/******************************************************************************
** Function: SBNMGR_OutputTelemetry
**
*/
uint16 SBNMGR_OutputPackets(void)
{

   static struct sockaddr_in SocketAddr;
   int                       SocketStatus;
   int32                     SbStatus;
   uint16                    PktLen;
   uint16                    AppId;
   uint16                    NumTxPktsOutput  = 0;
   uint32                    NumTxBytesOutput = 0;
   CFE_SB_Msg_t              *PktPtr;

   CFE_PSP_MemSet((void*)&SocketAddr, 0, sizeof(SocketAddr));
   SocketAddr.sin_family      = AF_INET;
   SocketAddr.sin_addr.s_addr = inet_addr(SbnMgr->TxDestIp);
   SocketAddr.sin_port        = htons(SBN_LITE_TX_PORT);
   SocketStatus = 0;

   /*
   ** CFE_SB_RcvMsg returns CFE_SUCCESS when it gets a packet, otherwise
   ** no packet was received
   */
   do
   {
      SbStatus = CFE_SB_RcvMsg(&PktPtr, SbnMgr->TxPipe, CFE_SB_POLL);

      if ( (SbStatus == CFE_SUCCESS) && (SbnMgr->SuppressSend == FALSE) ) {
          
         PktLen = CFE_SB_GetTotalMsgLength(PktPtr);

         if(SbnMgr->OutputEnable) {
            
            AppId = CFE_SB_GetMsgId(PktPtr) & SBNTBL_APP_ID_MASK;
            // OS_printf("sbn-lite: received appid %u PktLen %u\n", AppId, PktLen);
            
            if (!PktUtil_IsPacketFiltered(PktPtr, &(SbnMgr->Tbl.Pkt[AppId].Filter))) {

               SocketStatus = sendto(SbnMgr->TxSockId, (char *)PktPtr, PktLen, 0,
                                       (struct sockaddr *) &SocketAddr, sizeof(SocketAddr) );
                                       
               ++NumTxPktsOutput;
               NumTxBytesOutput += PktLen;

            } /* End if packet is not filtered */
         } /* End if downlink enabled */
          
         if (SocketStatus < 0) {
             
            CFE_EVS_SendEvent(SBNMGR_SOCKET_SEND_ERR_EID,CFE_EVS_ERROR,
                              "Error sending packet on socket %s, port %d, errno %d. Tlm output suppressed\n",
                              SbnMgr->TxDestIp, SBN_LITE_TX_PORT, errno);
            SbnMgr->SuppressSend = TRUE;
         }

      } /* End if SB received msg and output enabled */

   } while(SbStatus == CFE_SUCCESS);

   ComputeStats(NumTxPktsOutput, NumTxBytesOutput);

   return NumTxPktsOutput;
   
} /* End of SBNMGR_OutputTelemetry() */

/******************************************************************************
** Function: SBNMGR_ReadPackets
**
*/
int SBNMGR_ReadPackets(uint16 MaxMsgRead)
{

	unsigned int AddrLen = sizeof(SbnMgr->RxSockAddr);
   int i = 0;
   int Status;

   if (SbnMgr->RxBind == FALSE) return i;

   CFE_PSP_MemSet(&(SbnMgr->RxSockAddr), 0, sizeof(SbnMgr->RxSockAddr));

   for (i = 0; i < MaxMsgRead; i++) {

      Status = recvfrom(SbnMgr->RxSockId, (char *)&(SbnMgr->RecvBuff[i]), sizeof(SbnMgr->RecvBuff), MSG_DONTWAIT,
                       (struct sockaddr *) &(SbnMgr->RxSockAddr), &AddrLen);

      if ( (Status < 0) && (errno == EWOULDBLOCK) )
         break; /* no (more) messages */
      else {
            
         if (Status <= SBNMGR_RECV_BUFF_LEN) {

            CFE_EVS_SendEvent(SBNMGR_DEBUG_EID, CFE_EVS_DEBUG,
                              "SBNMGR Rx: Read %d bytes from socket\n",Status);
            SbnMgr->RxPktCnt++;

            //JRL - Validate checksum before sending?
            CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &(SbnMgr->RecvBuff[i]));
         }
         else {
            
            SbnMgr->RxPktErrCnt++;
            CFE_EVS_SendEvent(SBNMGR_RECV_LEN_ERR_EID,CFE_EVS_ERROR,
                              "Sbn-lite pkt dropped (too long). Header: 0x%02x%2x 0x%02x%2x 0x%02x%2x 0x%02x%2x",
                		         SbnMgr->RecvBuff[0], SbnMgr->RecvBuff[1], SbnMgr->RecvBuff[2], SbnMgr->RecvBuff[3],
                              SbnMgr->RecvBuff[4], SbnMgr->RecvBuff[5], SbnMgr->RecvBuff[6], SbnMgr->RecvBuff[7]);
         }
      } /* End if received a message */
   } /* End receive loop */

   return i;

} /* End SBNMGR_ReadPackets() */

/******************************************************************************
** Function: SBNMGR_LoadTbl
**
*/
boolean SBNMGR_LoadTbl(SBNTBL_Tbl* NewTbl)
{

   uint16   AppId;
   uint16   PktCnt = 0;
   uint16   FailedSubscription = 0;
   int32    Status;
   boolean  RetStatus = TRUE;

   CFE_SB_MsgPtr_t MsgPtr = NULL;

   SBNMGR_RemoveAllPktsCmd(NULL, MsgPtr);  /* Both parameters are unused so OK to be NULL */

   CFE_PSP_MemCpy(&(SbnMgr->Tbl), NewTbl, sizeof(SBNTBL_Tbl));

   for (AppId=0; AppId < SBNTBL_MAX_APP_ID; AppId++) {

      if (SbnMgr->Tbl.Pkt[AppId].StreamId != SBNTBL_UNUSED_MSG_ID) {
         
         ++PktCnt;
         Status = SubscribeNewPkt(&(SbnMgr->Tbl.Pkt[AppId])); 
         /*~~
         Status = CFE_SB_SubscribeEx(SbnMgr->Tbl.Pkt[AppId].StreamId,
                                     SbnMgr->TlmPipe,SbnMgr->Tbl.Pkt[AppId].Qos,
                                     SbnMgr->Tbl.Pkt[AppId].BufLim);
         */

         if(Status != CFE_SUCCESS) {
            
            ++FailedSubscription;
            CFE_EVS_SendEvent(SBNMGR_LOAD_TBL_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
                              "Error subscribing to stream 0x%04X, BufLim %d, Status %i",
                              SbnMgr->Tbl.Pkt[AppId].StreamId, SbnMgr->Tbl.Pkt[AppId].BufLim, Status);
         }
      }

   } /* End pkt loop */

   if (FailedSubscription == 0) {
      
      SBNMGR_InitStats(SBN_LITE_RUN_LOOP_DELAY_MS,SBNMGR_STATS_STARTUP_INIT_MS);
      CFE_EVS_SendEvent(SBNMGR_LOAD_TBL_INFO_EID, CFE_EVS_INFORMATION,
                        "Successfully loaded new table with %d packets", PktCnt);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(SBNMGR_LOAD_TBL_ERR_EID, CFE_EVS_INFORMATION,
                        "Attempted to load new table with %d packets. Failed %d subscriptions",
                        PktCnt, FailedSubscription);
   }

   return RetStatus;

} /* End SBNMGR_LoadTbl() */


/******************************************************************************
** Function: SBNMGR_LoadTblEntry
**
*/
boolean SBNMGR_LoadTblEntry(uint16 AppId, SBNTBL_Pkt* PktArray)
{

   int32    Status;
   boolean  RetStatus = TRUE;
   SBNTBL_Pkt* NewPkt = &(SbnMgr->Tbl.Pkt[AppId]); 

   CFE_PSP_MemCpy(NewPkt,&PktArray[AppId],sizeof(SBNTBL_Pkt));

   Status = SubscribeNewPkt(NewPkt);
   
   if(Status == CFE_SUCCESS) {
      
      CFE_EVS_SendEvent(SBNMGR_LOAD_TBL_ENTRY_INFO_EID, CFE_EVS_INFORMATION,
                        "Loaded table entry %d with stream 0x%04X, BufLim %d",
                        AppId, NewPkt->StreamId, NewPkt->BufLim);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(SBNMGR_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
                        "Error subscribing to stream 0x%04X, BufLim %d, Status %i",
                        NewPkt->StreamId, NewPkt->BufLim, Status);
   }

   return RetStatus;

} /* End SBNMGR_LoadTblEntry() */


// /******************************************************************************
// ** Function: SBNMGR_EnableOutputCmd
// **
// */
// boolean SBNMGR_EnableOutputCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
// {

//    const SBNMGR_EnableOutputCmdParam *EnableOutputCmd = (const SBNMGR_EnableOutputCmdParam *) MsgPtr;
//    boolean  RetStatus = TRUE;

//    strncpy(SbnMgr->TlmDestIp, EnableOutputCmd->DestIp, SBNMGR_IP_STR_LEN);

//    SbnMgr->SuppressSend = FALSE;
//    CFE_EVS_SendEvent(SBNMGR_TLM_OUTPUT_ENA_INFO_EID, CFE_EVS_INFORMATION,
//                      "Telemetry output enabled for IP %s", SbnMgr->TlmDestIp);

//    /*
//    ** If disabled then create the socket and turn it on. If already
//    ** enabled then destination address is changed in the existing socket
//    */
//    if(SbnMgr->DownlinkOn == FALSE) { 

//       if ( (SbnMgr->TlmSockId = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
         
//          RetStatus = FALSE;
//          CFE_EVS_SendEvent(SBNMGR_TLM_OUTPUT_ENA_SOCKET_ERR_EID, CFE_EVS_ERROR,
//                            "Telemetry output enable socket error. errno %d", errno);
//       }
//       else {
         
//          SBNMGR_InitStats(KIT_TO_RUN_LOOP_DELAY_MS,SBNMGR_STATS_STARTUP_INIT_MS);
//          SbnMgr->DownlinkOn = TRUE;
//       }

//    } /* End if downlink disabled */

//    return RetStatus;

// } /* End SBNMGR_EnableOutputCmd() */


/******************************************************************************
** Function: SBNMGR_AddPktCmd
**
** Notes:
**   1. Command rejected if table has existing entry for commanded Stream ID
**   2. Only update the table if the software bus subscription successful.  
** 
*/
boolean SBNMGR_AddPktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const SBNMGR_AddPktCmdParam *AddPktCmd = (const SBNMGR_AddPktCmdParam *) MsgPtr;
   SBNTBL_Pkt  NewPkt;
   boolean     RetStatus = TRUE;
   int32       Status;
   uint16      AppId;

   
   AppId = AddPktCmd->StreamId & SBNTBL_APP_ID_MASK;
   
   if (SbnMgr->Tbl.Pkt[AppId].StreamId == SBNTBL_UNUSED_MSG_ID) {
      
      NewPkt.StreamId     = AddPktCmd->StreamId;
      NewPkt.Qos          = AddPktCmd->Qos;
      NewPkt.BufLim       = AddPktCmd->BufLim;
      NewPkt.Filter.Type  = AddPktCmd->FilterType;
      NewPkt.Filter.Param = AddPktCmd->FilterParam;
   
      Status = SubscribeNewPkt(&NewPkt);
   
      if (Status == CFE_SUCCESS) {

         SbnMgr->Tbl.Pkt[AppId] = NewPkt;
      
         CFE_EVS_SendEvent(SBNMGR_ADD_PKT_SUCCESS_EID, CFE_EVS_INFORMATION,
                           "Added packet 0x%04X, QoS (%d,%d), BufLim %d",
                           NewPkt.StreamId, NewPkt.Qos.Priority, NewPkt.Qos.Reliability, NewPkt.BufLim);
      }
      else {
   
         CFE_EVS_SendEvent(SBNMGR_ADD_PKT_ERROR_EID, CFE_EVS_ERROR,
                           "Error adding packet 0x%04X. Software Bus subscription failed with return status 0x%8x",
                           AddPktCmd->StreamId, Status);
      }
   
   } /* End if packet entry unused */
   else {
   
      CFE_EVS_SendEvent(SBNMGR_ADD_PKT_ERROR_EID, CFE_EVS_ERROR,
                        "Error adding packet 0x%04X. Packet already exists in the packet table",
                        AddPktCmd->StreamId);
   }
   
   return RetStatus;

} /* End of SBNMGR_AddPktCmd() */


/******************************************************************************
** Function: SBNMGR_RemoveAllPktsCmd
**
** Notes:
**   1. The cFE to_lab code unsubscribes the command and send HK MIDs. I'm not
**      sure why this is done and I'm not sure how the command is used. This 
**      command is intended to help manage TO telemetry packets.
*/
boolean SBNMGR_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   uint16   AppId;
   uint16   PktCnt = 0;
   uint16   FailedUnsubscribe = 0;
   int32    Status;
   boolean  RetStatus = TRUE;

   for (AppId=0; AppId < SBNTBL_MAX_APP_ID; AppId++) {
      
      if (SbnMgr->Tbl.Pkt[AppId].StreamId != SBNTBL_UNUSED_MSG_ID ) {
          
         ++PktCnt;

         Status = CFE_SB_Unsubscribe(SbnMgr->Tbl.Pkt[AppId].StreamId, SbnMgr->TxPipe);
         if(Status != CFE_SUCCESS) {
             
            FailedUnsubscribe++;
            CFE_EVS_SendEvent(SBNMGR_REMOVE_ALL_PKTS_ERROR_EID, CFE_EVS_ERROR,
                              "Error removing stream id 0x%04X at table packet index %d. Unsubscribe status 0x%8X",
                              SbnMgr->Tbl.Pkt[AppId].StreamId, AppId, Status);
         }

         SBNTBL_SetPacketToUnused(&(SbnMgr->Tbl.Pkt[AppId]));

      } /* End if packet in use */

   } /* End AppId loop */

   CFE_EVS_SendEvent(SBN_LITE_INIT_DEBUG_EID, SBN_LITE_INIT_EVS_TYPE, "SBNMGR_RemoveAllPktsCmd() - About to flush pipe\n");
   FlushTlmPipe();
   CFE_EVS_SendEvent(SBN_LITE_INIT_DEBUG_EID, SBN_LITE_INIT_EVS_TYPE, "SBNMGR_RemoveAllPktsCmd() - Completed pipe flush\n");

   if (FailedUnsubscribe == 0) {
      
      CFE_EVS_SendEvent(SBNMGR_REMOVE_ALL_PKTS_SUCCESS_EID, CFE_EVS_INFORMATION,
                        "Removed %d table packet entries", PktCnt);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(SBNMGR_REMOVE_ALL_PKTS_ERROR_EID, CFE_EVS_INFORMATION,
                        "Attempted to remove %d packet entries. Failed %d unsubscribes",
                        PktCnt, FailedUnsubscribe);
   }

   return RetStatus;

} /* End of SBNMGR_RemoveAllPktsCmd() */


/*******************************************************************
** Function: SBNMGR_RemovePktCmd
**
*/
boolean SBNMGR_RemovePktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const SBNMGR_RemovePktCmdParam *RemovePktCmd = (const SBNMGR_RemovePktCmdParam *) MsgPtr;
   uint16   AppId;
   int32    Status;
   boolean  RetStatus = TRUE;
  
   
   AppId = RemovePktCmd->StreamId & SBNTBL_APP_ID_MASK;
  
   if ( SbnMgr->Tbl.Pkt[AppId].StreamId != SBNTBL_UNUSED_MSG_ID) {

      SBNTBL_SetPacketToUnused(&(SbnMgr->Tbl.Pkt[AppId]));
      
      Status = CFE_SB_Unsubscribe(RemovePktCmd->StreamId, SbnMgr->TxPipe);
      if(Status == CFE_SUCCESS)
      {
         CFE_EVS_SendEvent(SBNMGR_REMOVE_PKT_SUCCESS_EID, CFE_EVS_INFORMATION,
                           "Succesfully removed stream id 0x%04X from the packet table",
                           RemovePktCmd->StreamId);
      }
      else
      {
         RetStatus = FALSE;
         CFE_EVS_SendEvent(SBNMGR_REMOVE_PKT_ERROR_EID, CFE_EVS_ERROR,
                           "Removed packet 0x%04X from packet table, but SB unsubscribefailed with return status 0x%8x",
                           RemovePktCmd->StreamId, Status);
      }

   } /* End if found stream ID in table */
   else
   {

      CFE_EVS_SendEvent(SBNMGR_REMOVE_PKT_ERROR_EID, CFE_EVS_ERROR,
                        "Error removing stream id 0x%04X. Packet not defined in packet table.",
                        RemovePktCmd->StreamId);

   } /* End if didn't find stream ID in table */

   return RetStatus;

} /* End of SBNMGR_RemovePktCmd() */


/*******************************************************************
** Function: SBNMGR_SendPktTblTlmCmd
**
** Any command 
*/
boolean SBNMGR_SendPktTblTlmCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const SBNMGR_SendPktTblTlmCmdParam *SendPktTblTlmCmd = (const SBNMGR_SendPktTblTlmCmdParam *) MsgPtr;
   uint16      AppId;
   SBNTBL_Pkt* PktPtr;
   int32    Status;
  
  
   AppId  = SendPktTblTlmCmd->StreamId & SBNTBL_APP_ID_MASK;
   PktPtr = &(SbnMgr->Tbl.Pkt[AppId]);
   
   SbnMgr->PktTlm.StreamId = PktPtr->StreamId;
   SbnMgr->PktTlm.Qos      = PktPtr->Qos;
   SbnMgr->PktTlm.BufLim   = PktPtr->BufLim;

   SbnMgr->PktTlm.FilterType  = PktPtr->Filter.Type;
   SbnMgr->PktTlm.FilterParam = PktPtr->Filter.Param;

   CFE_SB_TimeStampMsg((CFE_SB_MsgPtr_t) &(SbnMgr->PktTlm));
   Status = CFE_SB_SendMsg((CFE_SB_Msg_t *)&(SbnMgr->PktTlm));

   return (Status == CFE_SUCCESS);

} /* End of SBNMGR_SendPktTblTlmCmd() */


/******************************************************************************
** Function: SBNMGR_UpdateFilterCmd
**
** Notes:
**   1. Command rejected if AppId packet entry has not been loaded 
**   2. The filter type is verified but the filter parameter values are not 
** 
*/
boolean SBNMGR_UpdateFilterCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const SBNMGR_UpdateFilterCmdParam *UpdateFilterCmd = (const SBNMGR_UpdateFilterCmdParam *) MsgPtr;
   boolean     RetStatus = FALSE;
   uint16      AppId;

   
   AppId = UpdateFilterCmd->StreamId & SBNTBL_APP_ID_MASK;
   
   if (SbnMgr->Tbl.Pkt[AppId].StreamId != SBNTBL_UNUSED_MSG_ID) {
      
      if (PktUtil_IsFilterTypeValid(UpdateFilterCmd->FilterType)) {
        
         PktUtil_Filter* TblFilter = &(SbnMgr->Tbl.Pkt[AppId].Filter);
         
         CFE_EVS_SendEvent(SBNMGR_UPDATE_FILTER_CMD_SUCCESS_EID, CFE_EVS_INFORMATION,
                           "Successfully changed 0x%04X's filter (Type,N,X,O) from (%d,%d,%d,%d) to (%d,%d,%d,%d)",
                           UpdateFilterCmd->StreamId,
                           TblFilter->Type, TblFilter->Param.N, TblFilter->Param.X, TblFilter->Param.O,
                           UpdateFilterCmd->FilterType,   UpdateFilterCmd->FilterParam.N,
                           UpdateFilterCmd->FilterParam.X,UpdateFilterCmd->FilterParam.O);
                           
         TblFilter->Type  = UpdateFilterCmd->FilterType;
         TblFilter->Param = UpdateFilterCmd->FilterParam;         
        
         RetStatus = TRUE;
      
      } /* End if valid packet filter type */
      else {
   
         CFE_EVS_SendEvent(SBNMGR_UPDATE_FILTER_CMD_ERR_EID, CFE_EVS_ERROR,
                           "Error updating filter for packet 0x%04X. Invalid filter type %d",
                           UpdateFilterCmd->StreamId, UpdateFilterCmd->FilterType);
      }
   
   } /* End if packet entry unused */
   else {
   
      CFE_EVS_SendEvent(SBNMGR_UPDATE_FILTER_CMD_ERR_EID, CFE_EVS_ERROR,
                        "Error updating filter for packet 0x%04X. Packet not in use",
                        UpdateFilterCmd->StreamId);
   }
   
   return RetStatus;

} /* End of SBNMGR_UpdateFilterCmd() */


/******************************************************************************
** Function: DestructorCallback
**
** This function is called when the app is killed. This should
** never occur but if it does this will close the network socket.
*/
static void DestructorCallback(void)
{

   CFE_EVS_SendEvent(SBNMGR_DESTRUCTOR_INFO_EID, CFE_EVS_INFORMATION, "Destructor callback -- Closing SBN_LITE Tx Network socket. Output Enable = %d\n", SbnMgr->OutputEnable);
   
   if (SbnMgr->OutputEnable) {
      
      OS_close(SbnMgr->TxSockId);
   
   }

   if (SbnMgr->RxBind) {
      OS_close(SbnMgr->RxSockId);
   }

} /* End DestructorCallback() */

/******************************************************************************
** Function: FlushTlmPipe
**
** Remove all of the packets from the input pipe.
**
*/
static void FlushTlmPipe(void)
{

   int32 Status;
   CFE_SB_MsgPtr_t MsgPtr = NULL;

   do
   {
      Status = CFE_SB_RcvMsg(&MsgPtr, SbnMgr->TxPipe, CFE_SB_POLL);

   } while(Status == CFE_SUCCESS);

} /* End FlushTlmPipe() */


/******************************************************************************
** Function: SubscribeNewPkt
**
*/
static int32 SubscribeNewPkt(SBNTBL_Pkt* NewPkt)
{

   int32 Status;

   Status = CFE_SB_SubscribeEx(NewPkt->StreamId, SbnMgr->TxPipe, NewPkt->Qos, NewPkt->BufLim);

   return Status;

} /* End SubscribeNewPkt(() */


/******************************************************************************
** Function:  ComputeStats
**
** Called each output telemetry cycle
*/
static void ComputeStats(uint16 PktsSent, uint32 BytesSent)
{

   uint32 DeltaTimeMicroSec;   
   CFE_TIME_SysTime_t CurrTime = CFE_TIME_GetTime();
   CFE_TIME_SysTime_t DeltaTime;
   
   if (SbnMgr->Stats.InitCycles > 0) {
   
      --SbnMgr->Stats.InitCycles;
      SbnMgr->Stats.PrevTime = CFE_TIME_GetTime();
      SbnMgr->Stats.State = SBNMGR_STATS_INIT_CYCLE;

   }
   else {
      
      DeltaTime = CFE_TIME_Subtract(CurrTime, SbnMgr->Stats.PrevTime);
      DeltaTimeMicroSec = CFE_TIME_Sub2MicroSecs(DeltaTime.Subseconds); 
      
      SbnMgr->Stats.IntervalMilliSecs += (double)DeltaTime.Seconds*1000.0 + (double)DeltaTimeMicroSec/1000.0;
      SbnMgr->Stats.TxIntervalPkts      += PktsSent;
      SbnMgr->Stats.TxIntervalBytes     += BytesSent;

      if (SbnMgr->Stats.IntervalMilliSecs >= SBNMGR_COMPUTE_STATS_INTERVAL_MS) {
      
         double Seconds = SbnMgr->Stats.IntervalMilliSecs/1000;
         
         CFE_EVS_SendEvent(SBNMGR_DEBUG_EID, CFE_EVS_DEBUG,
                           "IntervalSecs=%f, TxIntervalPkts=%d, TxIntervalBytes=%d\n",
                           Seconds,SbnMgr->Stats.TxIntervalPkts,SbnMgr->Stats.TxIntervalBytes);
        
         SbnMgr->Stats.TxAvgPktsPerSec  = (double)SbnMgr->Stats.TxIntervalPkts/Seconds;
         SbnMgr->Stats.TxAvgBytesPerSec = (double)SbnMgr->Stats.TxIntervalBytes/Seconds;
         
         /* Good enough running average that avoids overflow */
         if (SbnMgr->Stats.State == SBNMGR_STATS_INIT_CYCLE) {
     
            SbnMgr->Stats.State = SBNMGR_STATS_INIT_INTERVAL;
       
         }
         else {
            
            SbnMgr->Stats.State = SBNMGR_STATS_VALID;
            SbnMgr->Stats.TxAvgPktsPerSec  = (SbnMgr->Stats.TxAvgPktsPerSec  + SbnMgr->Stats.TxPrevIntervalAvgPkts) / 2.0; 
            SbnMgr->Stats.TxAvgBytesPerSec = (SbnMgr->Stats.TxAvgBytesPerSec + SbnMgr->Stats.TxPrevIntervalAvgBytes) / 2.0; 
  
         }
         
         SbnMgr->Stats.TxPrevIntervalAvgPkts  = SbnMgr->Stats.TxAvgPktsPerSec;
         SbnMgr->Stats.TxPrevIntervalAvgBytes = SbnMgr->Stats.TxAvgBytesPerSec;
         
         SbnMgr->Stats.IntervalMilliSecs = 0.0;
         SbnMgr->Stats.TxIntervalPkts      = 0;
         SbnMgr->Stats.TxIntervalBytes     = 0;
      
      } /* End if report cycle */
      
      SbnMgr->Stats.PrevTime = CFE_TIME_GetTime();
      
   } /* End if not init cycle */
   

} /* End ComputeStats() */
