/*
** Purpose: Implement a communication object.
**
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>
#include <errno.h>

#include "app_cfg.h"
#include "comm_obj.h"

/*
** Global File Data
*/

static COMM_OBJ_Class*  CommObj = NULL;

/*
** Local Function Prototypes
*/
static void DestructorCallback(void);

/******************************************************************************
** Function: COMM_OBJ_Constructor
**
*/
void COMM_OBJ_Constructor(COMM_OBJ_Class*  CommObjPtr)
{
 
   CommObj = CommObjPtr;

   CFE_PSP_MemSet((void*)CommObj, 0, sizeof(COMM_OBJ_Class));

   CFE_SB_CreatePipe(&(CommObj->TlmPipe), COMM_OBJ_PIPE_DEPTH, COMM_OBJ_PIPE_NAME);
   CFE_SB_Subscribe(COMM_SLA_RAW_MID, CommObj->TlmPipe);

   CFE_SB_InitMsg(&(CommObj->DemodPkt), COMM_DEMOD_TLM_MID, COMM_OBJ_DEMOD_LEN, TRUE);
   CFE_SB_InitMsg(&(CommObj->ModPkt), COMM_MOD_TLM_MID, COMM_OBJ_MOD_LEN, TRUE);
   CFE_SB_InitMsg(&(CommObj->KeyPkt), COMM_SLA_TLM_MID, COMM_OBJ_KeyPkt_LEN, TRUE);
 
   CommObj->SLA_Attribution_Key = 0xFFFFFFFF;
   sprintf(CommObj->CommStatusString, "----");

   OS_TaskInstallDeleteHandler((void *)(&DestructorCallback)); /* Call when application terminates */
    
} /* End COMM_OBJ_Constructor() */

/******************************************************************************
** Function: DestructorCallback
**
** This function is called when the uplink is terminated. This should
** never occur but if it does this will close the network socket.
*/
static void DestructorCallback(void)
{

   CFE_EVS_SendEvent(COMM_OBJ_DEMO_DEBUG_EID, CFE_EVS_DEBUG, "COMM_OBJ deleting callback.\n");

} /* End DestructorCallback() */

void COMM_OBJ_UpdatePingStatus(uint64 status) {
   CFE_SB_InitMsg(&(CommObj->StatusPkt), COMM_SLA_PING_MID, COMM_OBJ_PingStatus_LEN, TRUE);

   CFE_ES_WriteToSysLog ("COMM_OBJ_UpdatePingStatus: status: 0x%x", status);

   CommObj->SLA_Ping_Status = status;  // save status

   CommObj->StatusPkt.Header[0] = 0x09; // setting to be the message type COMM_SLA_PING_MID
   CommObj->StatusPkt.Header[1] = 0xfA;
   CommObj->StatusPkt.PingStatus = status;
 
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(CommObj->StatusPkt));
   CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &(CommObj->StatusPkt));
   CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_INFORMATION, "Sending Ping Update Packet via SB");
}

void COMM_OBJ_UpdateSLAKey(uint32 key1, uint32 key2){

   CFE_SB_InitMsg(&(CommObj->AttrPkt), COMM_SLA_TLM_MID, COMM_OBJ_AttrUpdatePkt_LEN, TRUE);
   //CFE_ES_WriteToSysLog ("COMM_OBJ_UpdateSLAKey: key1: %x key2: %x", key1, key2);

   CommObj->AttrPkt.Header[0] = 0x09; // setting to be the message type COMM_SLA_TLM_MID
   CommObj->AttrPkt.Header[1] = 0xf9;
   CommObj->AttrPkt.AttrKey1 = key1;
   CommObj->AttrPkt.AttrKey2 = key2;
 
   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(CommObj->AttrPkt));
   CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &(CommObj->AttrPkt));
   CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_INFORMATION, "Sending Attr Update Packet via SB");
}

void COMM_OBJ_ProcessSLA(uint16 PktLen, CFE_SB_Msg_t *PktPtr){
   char buf[16];

   CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_INFORMATION, "%x %x", buf, PktLen);

   // copy the data locally
   CFE_PSP_MemCpy(buf, PktPtr, PktLen);

   CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_INFORMATION, "p\n");
}

/******************************************************************************
** Function: COMM_OBJ_Execute
**
** Execute main object function.
**
*/
void COMM_OBJ_Execute(void)
{
   int Status;
   CFE_SB_MsgId_t            MsgId;
   CFE_SB_Msg_t              *PktPtr;
   uint16                    PktLen;
   void                      *Message;
   int                       ssz;
 
   CommObj->ExecCnt++;

   Status = CFE_SB_RcvMsg(&PktPtr, CommObj->TlmPipe, CFE_SB_POLL);

   //CFE_EVS_SendEvent (CFE_EVS_INFORMATION, CFE_EVS_INFORMATION, "Addresses: COMM_OBJ_ProcessSLA: 0x%x COMM_OBJ_UpdateSLAKey: 0x%x Incoming message: 0x%x", &COMM_OBJ_ProcessSLA, &COMM_OBJ_UpdateSLAKey, &Message);
   
   ssz = snprintf(CommObj->CommStatusString, COMM_MAX_STATUS_STRING_SIZE,
      "Addresses: COMM_OBJ_ProcessSLA: 0x%x COMM_OBJ_UpdateSLAKey: 0x%x Incoming message: 0x%x", 
      &COMM_OBJ_ProcessSLA, &COMM_OBJ_UpdateSLAKey, &Message);
   
   //CFE_ES_WriteToSysLog("COMM: string size: %d", ssz);

   if ( Status == CFE_SUCCESS ) {

      MsgId = CFE_SB_GetMsgId(PktPtr);
      PktLen = CFE_SB_GetTotalMsgLength(PktPtr);

      Message = (void *)PktPtr;

      // CFE_ES_WriteToSysLog ("COMM: Recv'd message from SB, MsgId: %x, PktLen: %d", MsgId, PktLen);

      for (int i = 0; i < 18; i++){
         CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_INFORMATION, "[%d]:%x",i,PktPtr->Byte[i]);
      }

      CFE_EVS_SendEvent (CFE_EVS_INFORMATION,
                   CFE_EVS_DEBUG,
                   "COMM received SB msg Status: %d ExecutionCount: %u",Status,CommObj->ExecCnt);
      //DMD packet
      if (MsgId == COMM_DMD_PKT_MSG_ID &&
          (PktPtr->Byte[2] == 0x44 && PktPtr->Byte[3] == 0x3A)) {
         CFE_PSP_MemCpy(&(CommObj->DemodPkt.Synch), PktPtr, PktLen);
         CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(CommObj->DemodPkt));
         CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &(CommObj->DemodPkt));
      }
      //MOD packet
      else if (MsgId == COMM_MOD_PKT_MSG_ID &&
          (PktPtr->Byte[2] == 0x44 && PktPtr->Byte[3] == 0x3A)) {
         CFE_PSP_MemCpy(&(CommObj->ModPkt.Synch), PktPtr, PktLen);
         CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &(CommObj->ModPkt));
         CFE_SB_SendMsg((CFE_SB_MsgPtr_t) &(CommObj->ModPkt));
      }
      // Payload SLA packet
      else if ( MsgId == COMM_SLA_RAW_MID && 
               ((COMM_OBJ_KeyPkt*) Message)->Synch == COMM_KEY_PKT_MSG_SYNC) {

         CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_INFORMATION, "received payload SLA TLM msg Status: %d ExecutionCount: %u",Status,CommObj->ExecCnt);
         //sprintf(CommObj->CommStatusString,"CommObj->SlaPkt address: %x",&(CommObj->KeyPkt));   // save for hk packet
         //CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_INFORMATION, CommObj->CommStatusString); 
         //CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_DEBUG, "AttrKey data: %llx, len: %d",CommObj->SLA_Attribution_Key, COMM_OBJ_AttrUpdatePkt_LEN);

         Message = (COMM_OBJ_KeyPkt *)PktPtr;

         //COMM_OBJ_UpdateSLAKey( ((COMM_OBJ_KeyPkt *)Message)->AttrKey );
 
         OS_TaskDelay(1);
         
         COMM_OBJ_ProcessSLA(PktLen, PktPtr);

         CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID, CFE_EVS_DEBUG, "After memcpy: AttrKey address: %x data: %llx",&(CommObj->SLA_Attribution_Key), CommObj->SLA_Attribution_Key);
      } 
      else if ( MsgId == COMM_SLA_RAW_MID && 
               ((COMM_OBJ_PingPkt*) Message)->Synch == COMM_PING_PKT_MSG_SYNC)  {

         Message = (COMM_OBJ_PingPkt *)PktPtr;     
         COMM_OBJ_UpdatePingStatus(((COMM_OBJ_PingPkt*) Message)->Status);

         // CFE_ES_WriteToSysLog ("COMM: Received ping packet: %x", ((COMM_OBJ_PingPkt*) Message)->Status);   
      } 
      else {
         CFE_EVS_SendEvent(COMM_OBJ_DEMO_DEBUG_EID, CFE_EVS_ERROR, 
                  "COMM_OBJ received an unknown packet from the radio, packet: Byte[0]: %x, Byte[1]: %x, Byte[12]: %x, Byte[13]: %x", 
                  PktPtr->Byte[0], PktPtr->Byte[1], PktPtr->Byte[12], PktPtr[13] );
      }
   }

      
} /* COMM_OBJ_Execute() */

/******************************************************************************
** Function:  COMM_OBJ_ResetStatus
**
*/
void COMM_OBJ_ResetStatus(void)
{

   CommObj->ExecCnt = 0;
   
} /* End COMM_OBJ_ResetStatus() */


/******************************************************************************
** Function: COMM_OBJ_DemoCmd
**
** Send an event message showing that the example object's command is executed.
**
*/
boolean COMM_OBJ_DemoCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const COMM_OBJ_DemoCmdMsg *CmdMsg = (const COMM_OBJ_DemoCmdMsg *) MsgPtr;

   CFE_EVS_SendEvent (COMM_OBJ_DEMO_CMD_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "Example demo command received with parameter %d",
                      CmdMsg->Parameter);

   return TRUE;

} /* End COMM_OBJ_EnableDataLoadCmd() */


/* end of file */
