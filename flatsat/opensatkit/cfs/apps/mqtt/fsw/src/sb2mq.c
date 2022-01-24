/*
** Purpose: Implement the SB-to-MQTT Table
**
** Notes:
**   1. Working in Fremework of existing app
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "sb2mq.h"
#include "mqtt_msg.h"

#define  JSON  &(Sb2Mq->Json)  /* Convenience macro */

/*
** Global File Data
*/

static SB2MQ_Class*  Sb2Mq = NULL;
static MQTT_CLIENT_Class* Sb2MqClient = NULL;
static uint16* MqttAppRunLoopMs = NULL;
static SB2MQ_Tbl tempTbl;


/*
** Local Function Prototypes
*/
// static void SubscribeToSbMessages(void);
// static int32 ValidateTbl(void* TblPtr);
static void SB2MQ_ComputeStats(uint16 PktsSent, uint32 BytesSent);
/*
** Local File Function Prototypes
*/
static boolean WriteJsonPkt(int32 FileHandle, const SB2MQ_TblEntry* Pkt, boolean FirstPktWritten);

/******************************************************************************
** Function: JSON Callbacks
**
** Notes:
**   1. These functions must have the same function signature as 
**      JSON_ContainerFuncPtr.
*/
static boolean PktCallback (void* UserData, int TokenIdx);
static boolean FilterCallback (void* UserData, int TokenIdx);

/******************************************************************************
** Function: SB2MQ_Constructor
**
*/
void SB2MQ_Constructor(SB2MQ_Class*  Sb2MqPtr,
                       MQTT_CLIENT_Class* Sb2MqClientPtr,
                       uint16* MqttAppRunLoopMsPtr,
                       SB2MQ_GetTblPtrFcn    GetTblPtrFunc,
                       SB2MQ_LoadTblFcn      LoadTblFunc, 
                       SB2MQ_LoadTblEntryFcn LoadTblEntryFunc,
                       const char* PipeName, 
                       uint16 PipeDepth)
{
 
   Sb2Mq = Sb2MqPtr;
   Sb2MqClient = Sb2MqClientPtr;
   MqttAppRunLoopMs = MqttAppRunLoopMsPtr;

   CFE_PSP_MemSet((void*)Sb2Mq, 0, sizeof(SB2MQ_Class));
   SB2MQ_SetTblToUnused(&(tempTbl));

   Sb2Mq->GetTblPtrFunc    = GetTblPtrFunc;
   Sb2Mq->LoadTblFunc      = LoadTblFunc;
   Sb2Mq->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON, Sb2Mq->JsonFileBuf, Sb2Mq->JsonFileTokens);
   
   JSON_ObjConstructor(&(Sb2Mq->JsonObj[SB2MQ_OBJ_PKT]),
                       SB2MQ_OBJ_NAME_PKT,
                       PktCallback,
                       (void *)&(Sb2Mq->Tbl.Pkt));

   JSON_RegContainerCallback(JSON, &(Sb2Mq->JsonObj[SB2MQ_OBJ_PKT]));
   
   JSON_ObjConstructor(&(Sb2Mq->JsonObj[SB2MQ_OBJ_FILTER]),
                       SB2MQ_OBJ_NAME_FILTER,
                       FilterCallback,
                       (void *)&(Sb2Mq->Tbl.Pkt));

   JSON_RegContainerCallback(JSON, &(Sb2Mq->JsonObj[SB2MQ_OBJ_FILTER]));

   CFE_SB_CreatePipe(&Sb2Mq->TxPipe, PipeDepth, PipeName);  

   // CFE_SB_InitMsg(&(Sb2Mq->PktTlm), PktTlmMsgId, SB2MQ_PKT_TLM_LEN, TRUE);


} /* End SB2MQ_Constructor() */


/******************************************************************************
** Function:  SB2MQ_ResetStatus
**
*/
void SB2MQ_ResetStatus()
{

   /* Nothing to do */

} /* End SB2MQ_ResetStatus() */


/******************************************************************************
** Function: FlushTlmPipe
**
** Remove all of the packets from the input pipe.
**
*/
static void FlushSbPipe(void)
{

   int32 Status;
   CFE_SB_MsgPtr_t MsgPtr = NULL;

   do
   {
      Status = CFE_SB_RcvMsg(&MsgPtr, Sb2Mq->TxPipe, CFE_SB_POLL);

   } while(Status == CFE_SUCCESS);

} /* End FlushTlmPipe() */



/******************************************************************************
** Function: SB2MQ_OutputPackets
**
*/
uint16 SB2MQ_OutputPackets(void)
{

   boolean                   PubStatus = false;
   int32                     SbStatus;
   uint16                    PktLen;
   CFE_SB_MsgId_t            AppId;
   uint16                    PubLen;
   uint16                    NumMqttMsgPublished  = 0;
   uint32                    NumMqttBytesPublised = 0;
   CFE_SB_Msg_t              *PktPtr;
   char                      MqttTopicPub[76];

   /*
   ** CFE_SB_RcvMsg returns CFE_SUCCESS when it gets a packet, otherwise
   ** no packet was received
   */
   do
   {
      SbStatus = CFE_SB_RcvMsg(&PktPtr, Sb2Mq->TxPipe, CFE_SB_POLL);

      if ( (SbStatus == CFE_SUCCESS) && (Sb2Mq->SuppressSend != FALSE) ) {
          
         PktLen = CFE_SB_GetTotalMsgLength(PktPtr);

         AppId = CFE_SB_GetMsgId(PktPtr) & SB2MQ_APP_ID_MASK;
         
         // OS_printf("sbn-lite: received appid %u PktLen %u\n", AppId, PktLen); 
         if(!MQTT_CLIENT_Connected(Sb2MqClient)) {
            CFE_EVS_SendEvent(SB2MQ_PUBLISH_STATUS_ERROR_EID, CFE_EVS_ERROR,
                     "MQTT Pub Client Dissconected, Reconnecting");
            MQTT_CLIENT_Disconnect(Sb2MqClient);
            OS_TaskDelay(2000);
            if(MQTT_CLIENT_NetworkInitAndConnect(Sb2MqClient) == 0) {
               MQTT_CLIENT_Connect(Sb2MqClient);
            } else {
               CFE_EVS_SendEvent(SB2MQ_PUBLISH_STATUS_ERROR_EID, CFE_EVS_ERROR,
                     "MQTT Pub Client Network Init Failed, What do I do now? :(");
            }
            
         }
         boolean isFiltered = PktUtil_IsPacketFiltered(PktPtr, &(Sb2Mq->Tbl.Pkt[AppId].Filter));
         boolean connected = MQTT_CLIENT_Connected(Sb2MqClient);
         if(!isFiltered && connected) {
            MQTT_MSG_SbPkt* SbToMqttMsgPtr = (MQTT_MSG_SbPkt*) PktPtr;
            if (PktLen <= MQTT_MSG_SB_PKT_LEN) {
               if (Sb2Mq->Tbl.Pkt[AppId].MqttIncludeCcsdsHeader) {
                  PubLen = PktLen;
                  snprintf(MqttTopicPub, sizeof(MqttTopicPub), "%s/%d", Sb2Mq->Tbl.Pkt[AppId].MqttTopic, AppId);
               } else {
                  PubLen = CFE_SB_GetUserDataLength(PktPtr);
                  snprintf(MqttTopicPub, sizeof(MqttTopicPub), "%s", Sb2Mq->Tbl.Pkt[AppId].MqttTopic);
               }
               PubStatus = MQTT_CLIENT_Publish(Sb2MqClient, MqttTopicPub, SbToMqttMsgPtr->Data, PubLen, Sb2Mq->Tbl.Pkt[AppId].MqttQos, Sb2Mq->Tbl.Pkt[AppId].MqttRetained);
               if (PubStatus) {
                  ++NumMqttMsgPublished;
                  NumMqttBytesPublised += PubLen;
                  CFE_EVS_SendEvent(SB2MQ_PUBLISH_STATUS_EID, CFE_EVS_INFORMATION,
                     "Sending packet to MQTT. AppId %d, Topic: %s, NumPktsPublished: %d, NumBytesPublished: %d", AppId, MqttTopicPub, NumMqttMsgPublished, NumMqttBytesPublised);
               } else {
                  CFE_EVS_SendEvent(SB2MQ_PUBLISH_STATUS_ERROR_EID,CFE_EVS_ERROR,
                                    "Error sending packet to MQTT. AppId %d", AppId);
               }
            }
         }
            // SbnMgr->SuppressSend = TRUE;        

      } /* End if SB received msg and output enabled */

   } while(SbStatus == CFE_SUCCESS);

   SB2MQ_ComputeStats(NumMqttMsgPublished, NumMqttBytesPublised);

   return NumMqttMsgPublished;
   
} /* End of SB2MQ_OutputPackets() */

/******************************************************************************
** Function: SubscribeNewPkt
**
*/
int32 SB2MQ_SubscribeNewPkt(SB2MQ_TblEntry* NewPkt)
{

   int32 Status;

   Status = CFE_SB_SubscribeEx(NewPkt->StreamId, Sb2Mq->TxPipe, NewPkt->Qos, NewPkt->BufLim);

   if (Status != CFE_SUCCESS) {
      CFE_EVS_SendEvent(SB2MQ_DEBUG_EID,CFE_EVS_ERROR,
            "SB Subscribe Error. Code: %d", Status);
   }

   return Status;

} /* End SubscribeNewPkt(() */


// static void SubscribeToSbMessages(void) {

//    int    i;
//    int    SubscribeErr = 0;
//    for (i = 0; i < MQTT_MQ2SB_TBL_ENTRIES; i++) {
//       if (SB2MQ_SubscribeNewPkt(&Sb2Mq->Tbl.Pkt[i]) != CFE_SUCCESS) {
//          ++SubscribeErr;
//          CFE_EVS_SendEvent(SB2MQ_LOAD_TBL_SUBSCRIBE_ERR_EID, CFE_EVS_ERROR,
//                            "Error subscribing to SB2MQ table entry %d, stream-id %d, topic %s, QOS %d",
//                            i,  Sb2Mq->Tbl.Pkt[i].StreamId,  Sb2Mq->Tbl.Pkt[i].MqttTopic, Sb2Mq->Tbl.Pkt[i].MqttQos);
//       }
//    } /* End for loop */
// }


/******************************************************************************
** Function:  SB2MQ_InitStats
**
** If TxOutputInterval==0 then retain current stats
** ComputeStats() logic assumes at least 1 init cycle
**
*/
void SB2MQ_InitStats(uint16 TxOutputInterval, uint16 InitDelay)
{
   
   if (TxOutputInterval != 0) Sb2Mq->Stats.TxOutputInterval = (double)TxOutputInterval;

   Sb2Mq->Stats.State = SB2MQ_STATS_INIT_CYCLE;
   Sb2Mq->Stats.InitCycles = (Sb2Mq->Stats.TxOutputInterval >= InitDelay) ? 1 : (double)InitDelay/Sb2Mq->Stats.TxOutputInterval;
            
   Sb2Mq->Stats.IntervalMilliSecs = 0.0;
   Sb2Mq->Stats.TxIntervalPkts = 0;
   Sb2Mq->Stats.TxIntervalBytes = 0;
      
   Sb2Mq->Stats.TxPrevIntervalAvgPkts  = 0.0;
   Sb2Mq->Stats.TxPrevIntervalAvgBytes = 0.0;
   
   Sb2Mq->Stats.TxAvgPktsPerSec  = 0.0;
   Sb2Mq->Stats.TxAvgBytesPerSec = 0.0;

} /* End SB2MQ_InitStats() */


/******************************************************************************
** Function:  ComputeStats
**
** Called each output telemetry cycle
*/
static void SB2MQ_ComputeStats(uint16 PktsSent, uint32 BytesSent)
{

   uint32 DeltaTimeMicroSec;   
   CFE_TIME_SysTime_t CurrTime = CFE_TIME_GetTime();
   CFE_TIME_SysTime_t DeltaTime;
   
   if (Sb2Mq->Stats.InitCycles > 0) {
   
      --Sb2Mq->Stats.InitCycles;
      Sb2Mq->Stats.PrevTime = CFE_TIME_GetTime();
      Sb2Mq->Stats.State = SB2MQ_STATS_INIT_CYCLE;

   }
   else {
      
      DeltaTime = CFE_TIME_Subtract(CurrTime, Sb2Mq->Stats.PrevTime);
      DeltaTimeMicroSec = CFE_TIME_Sub2MicroSecs(DeltaTime.Subseconds); 
      
      Sb2Mq->Stats.IntervalMilliSecs += (double)DeltaTime.Seconds*1000.0 + (double)DeltaTimeMicroSec/1000.0;
      Sb2Mq->Stats.TxIntervalPkts      += PktsSent;
      Sb2Mq->Stats.TxIntervalBytes     += BytesSent;

      if (Sb2Mq->Stats.IntervalMilliSecs >= SB2MQ_COMPUTE_STATS_INTERVAL_MS) {
      
         double Seconds = Sb2Mq->Stats.IntervalMilliSecs/1000;
         
         CFE_EVS_SendEvent(SB2MQ_DEBUG_EID, CFE_EVS_DEBUG,
                           "IntervalSecs=%f, TxIntervalPkts=%d, TxIntervalBytes=%d\n",
                           Seconds,Sb2Mq->Stats.TxIntervalPkts,Sb2Mq->Stats.TxIntervalBytes);
        
         Sb2Mq->Stats.TxAvgPktsPerSec  = (double)Sb2Mq->Stats.TxIntervalPkts/Seconds;
         Sb2Mq->Stats.TxAvgBytesPerSec = (double)Sb2Mq->Stats.TxIntervalBytes/Seconds;
         
         /* Good enough running average that avoids overflow */
         if (Sb2Mq->Stats.State == SB2MQ_STATS_INIT_CYCLE) {
     
            Sb2Mq->Stats.State = SB2MQ_STATS_INIT_INTERVAL;
       
         }
         else {
            
            Sb2Mq->Stats.State = SB2MQ_STATS_VALID;
            Sb2Mq->Stats.TxAvgPktsPerSec  = (Sb2Mq->Stats.TxAvgPktsPerSec  + Sb2Mq->Stats.TxPrevIntervalAvgPkts) / 2.0; 
            Sb2Mq->Stats.TxAvgBytesPerSec = (Sb2Mq->Stats.TxAvgBytesPerSec + Sb2Mq->Stats.TxPrevIntervalAvgBytes) / 2.0; 
  
         }
         
         Sb2Mq->Stats.TxPrevIntervalAvgPkts  = Sb2Mq->Stats.TxAvgPktsPerSec;
         Sb2Mq->Stats.TxPrevIntervalAvgBytes = Sb2Mq->Stats.TxAvgBytesPerSec;
         
         Sb2Mq->Stats.IntervalMilliSecs = 0.0;
         Sb2Mq->Stats.TxIntervalPkts      = 0;
         Sb2Mq->Stats.TxIntervalBytes     = 0;
      
      } /* End if report cycle */
      
      Sb2Mq->Stats.PrevTime = CFE_TIME_GetTime();
      
   } /* End if not init cycle */
   

} /* End ComputeStats() */

/******************************************************************************
** Function: SB2MQ_AddPktCmd
**
** Notes:
**   1. Command rejected if table has existing entry for commanded Stream ID
**   2. Only update the table if the software bus subscription successful.  
** 
*/
boolean SB2MQ_AddPktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const SB2MQ_AddPktCmdParam *AddPktCmd = (const SB2MQ_AddPktCmdParam *) MsgPtr;
   SB2MQ_TblEntry  NewPkt;
   boolean     RetStatus = TRUE;
   int32       Status;

   if (Sb2Mq->Tbl.Pkt[AddPktCmd->NewPkt.StreamId].StreamId == SB2MQ_UNUSED_MSG_ID) {

      Status = SB2MQ_SubscribeNewPkt(&NewPkt);
   
      if (Status == CFE_SUCCESS) {

         CFE_PSP_MemCpy(&Sb2Mq->Tbl.Pkt[AddPktCmd->NewPkt.StreamId], &NewPkt, sizeof(SB2MQ_TblEntry));
      
         CFE_EVS_SendEvent(SB2MQ_ADD_PKT_SUCCESS_EID, CFE_EVS_INFORMATION,
                           "Added packet 0x%04X, SB QoS (%d,%d), BufLim %d, MQTT Topic %s, MQTT QoS, %d, MQTT Retatined %d", 
                           NewPkt.StreamId, NewPkt.Qos.Priority, NewPkt.Qos.Reliability, NewPkt.BufLim, NewPkt.MqttTopic, NewPkt.MqttQos, NewPkt.MqttRetained);
      }
      else {
   
         CFE_EVS_SendEvent(SB2MQ_ADD_PKT_ERROR_EID, CFE_EVS_ERROR,
                           "Error adding packet 0x%04X. Software Bus subscription failed with return status 0x%8x",
                           AddPktCmd->NewPkt.StreamId, Status);
      }
   
   } /* End if packet entry unused */
   else {
   
      CFE_EVS_SendEvent(SB2MQ_ADD_PKT_ERROR_EID, CFE_EVS_ERROR,
                        "Error adding packet 0x%04X. Packet already exists in the packet table",
                        AddPktCmd->NewPkt.StreamId);
   }
   
   return RetStatus;

} /* End of SB2MQ_AddPktCmd() */

/*******************************************************************
** Function: SB2MQ_RemovePktCmd
**
*/
boolean SB2MQ_RemovePktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const SB2MQ_RemovePktCmdParam *RemovePktCmd = (const SB2MQ_RemovePktCmdParam *) MsgPtr;
   uint16   StreamId;
   int32    Status;
   boolean  RetStatus = TRUE;
  
   
   StreamId = RemovePktCmd->StreamId & SB2MQ_APP_ID_MASK;
  
   if ( Sb2Mq->Tbl.Pkt[StreamId].StreamId != SB2MQ_UNUSED_MSG_ID) {

      SB2MQ_SetPacketToUnused(&(Sb2Mq->Tbl.Pkt[StreamId]));
      
      Status = CFE_SB_Unsubscribe(Sb2Mq->Tbl.Pkt[StreamId].StreamId, Sb2Mq->TxPipe);
      if(Status == CFE_SUCCESS)
      {
         CFE_EVS_SendEvent(SB2MQ_REMOVE_PKT_SUCCESS_EID, CFE_EVS_INFORMATION,
                           "Succesfully removed stream id 0x%04X from the packet table",
                           RemovePktCmd->StreamId);
      }
      else
      {
         RetStatus = FALSE;
         CFE_EVS_SendEvent(SB2MQ_REMOVE_PKT_ERROR_EID, CFE_EVS_ERROR,
                           "Removed packet 0x%04X from packet table, but SB unsubscribefailed with return status 0x%8x",
                           RemovePktCmd->StreamId, Status);
      }

   } /* End if found stream ID in table */
   else
   {

      CFE_EVS_SendEvent(SB2MQ_REMOVE_PKT_ERROR_EID, CFE_EVS_ERROR,
                        "Error removing stream id 0x%04X. Packet not defined in packet table.",
                        RemovePktCmd->StreamId);

   } /* End if didn't find stream ID in table */

   return RetStatus;

} /* End of SB2MQ_RemovePktCmd() */


/******************************************************************************
** Function: SB2MQ_RemoveAllPktsCmd
**
** Notes:
**   1. The cFE to_lab code unsubscribes the command and send HK MIDs. I'm not
**      sure why this is done and I'm not sure how the command is used. This
**      comman
*/
boolean SB2MQ_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   uint16   TblIdx;
   uint16   PktCnt = 0;
   uint16   FailedUnsubscribe = 0;
   int32    Status;
   boolean  RetStatus = TRUE;

   for (TblIdx=0; TblIdx < SB2MQ_MAX_APP_ID; TblIdx++) {
      
      if (Sb2Mq->Tbl.Pkt[TblIdx].StreamId != SB2MQ_UNUSED_MSG_ID ) {
          
         ++PktCnt;

         Status = CFE_SB_Unsubscribe(Sb2Mq->Tbl.Pkt[TblIdx].StreamId, Sb2Mq->TxPipe);
         if(Status != CFE_SUCCESS) {
             
            FailedUnsubscribe++;
            CFE_EVS_SendEvent(SB2MQ_REMOVE_ALL_PKTS_ERROR_EID, CFE_EVS_ERROR,
                              "Error removing stream id 0x%04X at table packet index %d. Unsubscribe status 0x%8X",
                              Sb2Mq->Tbl.Pkt[TblIdx].StreamId, TblIdx, Status);
         }
         // SB2MQ_SetPacketToUnused(&(Sb2Mq->Tbl.Pkt[TblIdx]));

      } /* End if packet in use */

   } /* End AppId loop */

   CFE_EVS_SendEvent(SB2MQ_INIT_DEBUG_EID, CFE_EVS_INFORMATION, "SB2MQ_RemoveAllPktsCmd() - About to flush pipe");
   FlushSbPipe();
   CFE_EVS_SendEvent(SB2MQ_INIT_DEBUG_EID, CFE_EVS_INFORMATION, "SB2MQ_RemoveAllPktsCmd() - Completed pipe flush");

   if (FailedUnsubscribe == 0) {
      
      CFE_EVS_SendEvent(SB2MQ_REMOVE_ALL_PKTS_SUCCESS_EID, CFE_EVS_INFORMATION,
                        "Removed %d table packet entries", PktCnt);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(SB2MQ_REMOVE_ALL_PKTS_ERROR_EID, CFE_EVS_INFORMATION,
                        "Attempted to remove %d packet entries. Failed %d unsubscribes",
                        PktCnt, FailedUnsubscribe);
   }

   return RetStatus;

} /* End of SB2MQ_RemoveAllPktsCmd() */

/******************************************************************************
** Function: SB2MQ_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean SB2MQ_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int AppId;
   
   CFE_EVS_SendEvent(SB2MQ_INIT_DEBUG_EID, SB2MQ_INIT_EVS_TYPE, "SB2MQ_LoadCmd() Entry. sizeof(Sb2Mq->Tbl) = %d\n", sizeof(Sb2Mq->Tbl));
   
   /* 
   ** Reset status, object modified flags, and data. A non-zero BufLim
   ** value is used to determine whether a packet was loaded.
   */
   SB2MQ_ResetStatus();  
   
   SB2MQ_SetTblToUnused(&(Sb2Mq->Tbl));

   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(SB2MQ_INIT_DEBUG_EID, SB2MQ_INIT_EVS_TYPE, "SB2MQ_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
    
      JSON_ProcessTokens(JSON);

      /* 
      ** Only process command if no attribute errors. No need to send an event
      ** message if there are attribute errors since events are sent for each
      ** error.  Checking at least one packet is a simple check but that's all
      ** that can be done because the table doesn't have any other constraints.
      ** A table with no packets is considered erroneous.
      */
      if (Sb2Mq->AttrErrCnt == 0) {
      
         if (Sb2Mq->PktLoadCnt > 0) {
         
            if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
			      Sb2Mq->LastLoadStatus = ((Sb2Mq->LoadTblFunc)(&(tempTbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

	         } /* End if replace entire table */
            else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
		         Sb2Mq->LastLoadStatus = TBLMGR_STATUS_VALID;
   
               if (Sb2Mq->JsonObj[SB2MQ_OBJ_PKT].Modified) {
                         
                  for (AppId=0; (AppId < SB2MQ_MAX_APP_ID) && (Sb2Mq->LastLoadStatus == TBLMGR_STATUS_VALID); AppId++) {
                          
                     if (Sb2Mq->Tbl.Pkt[AppId].StreamId != SB2MQ_UNUSED_MSG_ID) {

                        if (!(Sb2Mq->LoadTblEntryFunc)(AppId, (SB2MQ_TblEntry*)Sb2Mq->JsonObj[SB2MQ_OBJ_PKT].UserData))
                            Sb2Mq->LastLoadStatus = TBLMGR_STATUS_INVALID;
                     }     
                  } /* End packet array loop */                
               } /* End if at least one object modified */
               else {
                  
                  /* This is an un explainable error */
                  CFE_EVS_SendEvent(SB2MQ_LOAD_EMPTY_ERR_EID, CFE_EVS_ERROR, "Load packet table update rejected.");

               } /* End if no objects in file */
               
            } /* End if update records */
			   else {
               
               CFE_EVS_SendEvent(SB2MQ_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"Load packet table rejected. Invalid table command load type %d", LoadType);            
            
            } /* End if invalid command option */ 
            
         } /* End if at least one packet loaded */
         else {
            
            CFE_EVS_SendEvent(SB2MQ_LOAD_UPDATE_ERR_EID, CFE_EVS_ERROR,
			                     "Load packet table command rejected. %s didn't contain any packet defintions", Filename);
         
         } /* End if too many packets in table file */

         if (Sb2Mq->PktLoadCnt != Sb2Mq->FilterLoadCnt) {
            
            CFE_EVS_SendEvent(SB2MQ_LOAD_UNDEF_FILTERS_EID, CFE_EVS_INFORMATION, 
                              "Packet table loaded with %d packets and %d undefined filters",
                              Sb2Mq->PktLoadCnt, (Sb2Mq->PktLoadCnt-Sb2Mq->FilterLoadCnt));
                              
         }
      } /* End if no attribute errors */
            
   } /* End if valid file */
   else {
      
      //printf("**ERROR** Processing Packet Table file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(SB2MQ_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"Load packet table open failure for file %s. File Status = %s JSMN Status = %s",
	                     Filename, JSON_GetFileStatusStr(Sb2Mq->Json.FileStatus), JSON_GetJsmnErrStr(Sb2Mq->Json.JsmnStatus));
   
   } /* End if file processing error */

    
   return (Sb2Mq->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of SB2MQ_LoadCmd() */


/******************************************************************************
** Function: SB2MQ_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. DumpType is unused.
**  4. File is formatted so it can be used as a load file. It does not follow
**     the cFE table file format. 
**  5. Creates a new dump file, overwriting anything that may have existed
**     previously
*/

boolean SB2MQ_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   boolean  FirstPktWritten = FALSE;
   int32    FileHandle;
   uint16   AppId;
   char     DumpRecord[256];
   const SB2MQ_Tbl *Sb2MqPtr;
   char SysTimeStr[256];
   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      Sb2MqPtr = (Sb2Mq->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"MQTT SB2MQ Packet Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      
      sprintf(DumpRecord,"\"description\": \"SBN2MQ Tbl dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));


      /* 
      ** Packet Array 
      **
      ** - Not all fields in ground table are saved in FSW so they are not
      **   populated in the dump file. However, the dump file can still
      **   be loaded.
      */
      
      sprintf(DumpRecord,"\"packet-array\": [\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      for (AppId=0; AppId < SB2MQ_MAX_APP_ID; AppId++) {
               
         if (WriteJsonPkt(FileHandle, &(Sb2MqPtr->Pkt[AppId]), FirstPktWritten)) FirstPktWritten = TRUE;
              
      } /* End packet loop */

      sprintf(DumpRecord,"\n]}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(SB2MQ_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of SB2MQ_DumpCmd() */



/******************************************************************************
** Function: WriteJsonPkt
**
** Notes:
**   1. Can't end last record with a comma so logic checks that commas only
**      start to be written after the first packet has been written
*/
static boolean WriteJsonPkt(int32 FileHandle, const SB2MQ_TblEntry* Pkt, boolean FirstPktWritten) {
   
   boolean PktWritten = FALSE;
   char DumpRecord[256];

   if (Pkt->StreamId != SB2MQ_UNUSED_MSG_ID) {
      
      if (FirstPktWritten) {
         sprintf(DumpRecord,",\n");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      }
      
      sprintf(DumpRecord,"\"sb-packet\": {\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"   \"dec-id\": %d,\n   \"priority\": %d,\n   \"reliability\": %d,\n   \"buf-limit\": %d,\n   \"mqtt-topic\": %s,\n   \"mqtt-qos\": %d,\n   \"mqtt-retained\": %d,\n  \"mqtt-include-ccsds\": %d,\n",
              Pkt->StreamId, Pkt->Qos.Priority, Pkt->Qos.Reliability, Pkt->BufLim, Pkt->MqttTopic, Pkt->MqttQos, Pkt->MqttRetained, Pkt->MqttIncludeCcsdsHeader);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      sprintf(DumpRecord,"   \"filter\": { \"type\": %d, \"X\": %d, \"N\": %d, \"O\": %d}\n}",
              Pkt->Filter.Type, Pkt->Filter.Param.X, Pkt->Filter.Param.N, Pkt->Filter.Param.O);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
   
      PktWritten = TRUE;
      
   } /* End if StreamId record has been loaded */
   
   return PktWritten;
   
} /* End WriteJsonPkt() */


/******************************************************************************
** Function: PktCallback
**
** Process a packet table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. ObjLoadCnt incremented for every packet, valid or invalid.
**      PktLoadIdx index to stored new pkt and incremented for valid packets
*/
static boolean PktCallback (void* UserData, int TokenIdx)
{

   int  AttributeCnt = 0;
   int  JsonIntData;
   char JsonStrData[MQTT_TOPIC_LEN];
   SB2MQ_TblEntry Pkt;
   
   Sb2Mq->JsonObj[SB2MQ_OBJ_PKT].Modified = FALSE; 
   
   CFE_EVS_SendEvent(SB2MQ_INIT_DEBUG_EID, SB2MQ_INIT_EVS_TYPE,
                     "SB2MQ.PktCallback: PktLoadCnt %d, AttrErrCnt %d, TokenIdx %d",
                     Sb2Mq->PktLoadCnt, Sb2Mq->AttrErrCnt, TokenIdx);
      
   SB2MQ_SetPacketToUnused(&Pkt); /* Default filter to always filtered in case not loaded by FilterCallback() */
  
   if (JSON_GetValShortInt(JSON, TokenIdx, "dec-id",      &JsonIntData)) { AttributeCnt++; Pkt.StreamId        = (CFE_SB_MsgId_t) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "priority",    &JsonIntData)) { AttributeCnt++; Pkt.Qos.Priority    = (uint8) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "reliability", &JsonIntData)) { AttributeCnt++; Pkt.Qos.Reliability = (uint8) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "buf-limit",   &JsonIntData)) { AttributeCnt++; Pkt.BufLim          = (uint16)JsonIntData; }
   if (JSON_GetValStr(JSON, TokenIdx, "mqtt-topic", JsonStrData)) { AttributeCnt++; CFE_PSP_MemCpy(Pkt.MqttTopic, JsonStrData, MQTT_TOPIC_LEN);}
   if (JSON_GetValShortInt(JSON, TokenIdx, "mqtt-qos",   &JsonIntData)) { AttributeCnt++; Pkt.MqttQos          = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "mqtt-retained",   &JsonIntData)) { AttributeCnt++; Pkt.MqttRetained = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "mqtt-include-ccsds",   &JsonIntData)) { AttributeCnt++; Pkt.MqttIncludeCcsdsHeader = (uint16)JsonIntData; }

   if (AttributeCnt == 8) {
   
      ++Sb2Mq->PktLoadCnt;
      
      Sb2Mq->CurAppId = Pkt.StreamId & SB2MQ_APP_ID_MASK;     
      CFE_PSP_MemCpy(&tempTbl.Pkt[Sb2Mq->CurAppId], &Pkt, sizeof(SB2MQ_TblEntry));   
      
      Sb2Mq->JsonObj[SB2MQ_OBJ_PKT].Modified = TRUE;
      
      CFE_EVS_SendEvent(SB2MQ_INIT_DEBUG_EID, SB2MQ_INIT_EVS_TYPE, 
                        "SB2MQ.PktCallback (Stream ID, BufLim, Priority, Reliability, MQTT Topic, MQTT QoS, MQTT Retained, MQTT Include CCSDS Header): %d, %d, %d, %d, %s, %d, %d, %d",
                        Pkt.StreamId, Pkt.Qos.Priority, Pkt.Qos.Reliability, Pkt.BufLim, Pkt.MqttTopic, Pkt.MqttQos, Pkt.MqttRetained, Pkt.MqttIncludeCcsdsHeader);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ++Sb2Mq->AttrErrCnt;     
      CFE_EVS_SendEvent(SB2MQ_LOAD_PKT_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be 8.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return Sb2Mq->JsonObj[SB2MQ_OBJ_PKT].Modified;

} /* PktCallback() */


/******************************************************************************
** Function: FilterCallback
**
** Process a filter table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. ObjLoadCnt incremented for every packet, valid or invalid.
**      PktLoadIdx index to stored new pkt and incremented for valid packets
**   3. Filter must be defined within a packet structure and this code assumes
**      the filter corresponds to most recent packet callback
*/
static boolean FilterCallback (void* UserData, int TokenIdx)
{

   int  AttributeCnt = 0;
   int  JsonIntData;     
   PktUtil_Filter Filter;
   
   Sb2Mq->JsonObj[SB2MQ_OBJ_FILTER].Modified = FALSE;
   
   CFE_EVS_SendEvent(SB2MQ_INIT_DEBUG_EID, CFE_EVS_DEBUG,
                     "SB2MQ.FilterCallback: Current ApId 0x%04X, ObjLoadCnt %d, AttrErrCnt %d, TokenIdx %d",
                     Sb2Mq->CurAppId, Sb2Mq->PktLoadCnt, Sb2Mq->AttrErrCnt, TokenIdx);

   if (JSON_GetValShortInt(JSON, TokenIdx, "type", &JsonIntData)) { AttributeCnt++; Filter.Type    = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "X",    &JsonIntData)) { AttributeCnt++; Filter.Param.X = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "N",    &JsonIntData)) { AttributeCnt++; Filter.Param.N = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "O",    &JsonIntData)) { AttributeCnt++; Filter.Param.O = (uint16)JsonIntData; }
   
   
   if (AttributeCnt == 4) {
   
      ++Sb2Mq->FilterLoadCnt;

      CFE_PSP_MemCpy(&tempTbl.Pkt[Sb2Mq->CurAppId].Filter, &Filter, sizeof(PktUtil_Filter));   

      Sb2Mq->JsonObj[SB2MQ_OBJ_FILTER].Modified = TRUE;
               
      CFE_EVS_SendEvent(SB2MQ_INIT_DEBUG_EID, CFE_EVS_DEBUG, 
                        "SB2MQ.FilterCallback (Type, X, N, O): %d, %d, %d, %d",
                        Filter.Type, Filter.Param.X, Filter.Param.N, Filter.Param.O);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ++Sb2Mq->AttrErrCnt;     
      CFE_EVS_SendEvent(SB2MQ_LOAD_PKT_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be 4.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return Sb2Mq->JsonObj[SB2MQ_OBJ_FILTER].Modified;

} /* FilterCallback() */

/******************************************************************************
** Function: SB2MQ_GetTblPtr
**
*/
const SB2MQ_Tbl* SB2MQ_GetTblPtr()
{

   return &(Sb2Mq->Tbl);

} /* End SB2MQ_GetTblPtr() */


/******************************************************************************
** Function: SB2MQ_LoadTbl
**
*/
boolean SB2MQ_LoadTbl(SB2MQ_Tbl* NewTbl)
{

   uint16   AppId;
   uint16   PktCnt = 0;
   uint16   FailedSubscription = 0;
   int32    Status;
   boolean  RetStatus = TRUE;

   CFE_SB_MsgPtr_t MsgPtr = NULL;

   SB2MQ_RemoveAllPktsCmd(NULL, MsgPtr);  /* Both parameters are unused so OK to be NULL */

   CFE_PSP_MemCpy(&(Sb2Mq->Tbl), NewTbl, sizeof(SB2MQ_Tbl));

   for (AppId=0; AppId < SB2MQ_MAX_APP_ID; AppId++) {

      if (Sb2Mq->Tbl.Pkt[AppId].StreamId != SB2MQ_UNUSED_MSG_ID) {
         
         ++PktCnt;
         Status = SB2MQ_SubscribeNewPkt(&(Sb2Mq->Tbl.Pkt[AppId])); 
         /*~~
         Status = CFE_SB_SubscribeEx(Sb2Mq->Tbl.Pkt[AppId].StreamId,
                                     Sb2Mq->TlmPipe,Sb2Mq->Tbl.Pkt[AppId].Qos,
                                     Sb2Mq->Tbl.Pkt[AppId].BufLim);
         */

         if(Status != CFE_SUCCESS) {
            
            ++FailedSubscription;
            CFE_EVS_SendEvent(SB2MQ_LOAD_TBL_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
                              "Error subscribing to stream 0x%04X, BufLim %d, Status %i",
                              Sb2Mq->Tbl.Pkt[AppId].StreamId, Sb2Mq->Tbl.Pkt[AppId].BufLim, Status);
         }
      }

   } /* End pkt loop */

   if (FailedSubscription == 0) {
      
      SB2MQ_InitStats(*MqttAppRunLoopMs,MQTT_STATS_STARTUP_INIT_MS);
      CFE_EVS_SendEvent(SB2MQ_LOAD_TBL_INFO_EID, CFE_EVS_INFORMATION,
                        "Successfully loaded new table with %d packets", PktCnt);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(SB2MQ_LOAD_TBL_ERR_EID, CFE_EVS_INFORMATION,
                        "Attempted to load new table with %d packets. Failed %d subscriptions",
                        PktCnt, FailedSubscription);
   }

   return RetStatus;

} /* End SB2MQ_LoadTbl() */

/******************************************************************************
** Function: SB2MQ_LoadTblEntry
**
*/
boolean SB2MQ_LoadTblEntry(uint16 AppId, SB2MQ_TblEntry* PktArray)
{

   int32    Status;
   boolean  RetStatus = TRUE;
   SB2MQ_TblEntry* NewPkt = &(Sb2Mq->Tbl.Pkt[AppId]); 

   CFE_PSP_MemCpy(NewPkt,&PktArray[AppId],sizeof(SB2MQ_TblEntry));

   Status = SB2MQ_SubscribeNewPkt(NewPkt);
   
   if(Status == CFE_SUCCESS) {
      
      CFE_EVS_SendEvent(SB2MQ_LOAD_TBL_ENTRY_INFO_EID, CFE_EVS_INFORMATION,
                        "Loaded table entry %d with stream 0x%04X, BufLim %d",
                        AppId, NewPkt->StreamId, NewPkt->BufLim);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(SB2MQ_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
                        "Error subscribing to stream 0x%04X, BufLim %d, Status %i",
                        NewPkt->StreamId, NewPkt->BufLim, Status);
   }

   return RetStatus;

} /* End SB2MQ_LoadTblEntry() */

/******************************************************************************
** Function: SB2MQ_SetPacketToUnused
**
**
*/
void SB2MQ_SetPacketToUnused(SB2MQ_TblEntry* PktPtr)
{

   CFE_PSP_MemSet(PktPtr, 0, sizeof(SB2MQ_TblEntry));
   PktPtr->StreamId    = SB2MQ_UNUSED_MSG_ID;
   PktPtr->Filter.Type = PKTUTIL_FILTER_ALWAYS;
   
} /* End SBNTBL_SetPacketToUnused() */

/******************************************************************************
** Function: SB2MQ_SetTblToUnused
**
**
*/
void SB2MQ_SetTblToUnused(SB2MQ_Tbl* TblPtr)
{
  
   uint16 AppId;
   
   CFE_PSP_MemSet(TblPtr, 0, sizeof(SB2MQ_Tbl));

   for (AppId=0; AppId < SB2MQ_MAX_APP_ID; AppId++) {
      
      TblPtr->Pkt[AppId].StreamId    = SB2MQ_UNUSED_MSG_ID;
      TblPtr->Pkt[AppId].Filter.Type = PKTUTIL_FILTER_ALWAYS;
   
   }
   
} /* End SB2MQ_SetTblToUnused() */
