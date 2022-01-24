/* 
** Purpose: Implement a Sla_tlm application.
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "sla_tlm_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

SLA_TLM_Class  Sla_tlm;
SLA_TLM_HkPkt  Sla_tlmHkPkt;
SLA_TLM_AttrKeyPkt Sla_tlmKeyPkt;
SLA_TLM_PingPkt Sla_tlmPingPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(Sla_tlm.CmdMgr))

/******************************************************************************
** Function: SLA_TLM_AppMain
**
*/
void SLA_TLM_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;


   CFE_ES_PerfLogEntry(SLA_TLM_MAIN_PERF_ID);
   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);


   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS)
   {
       Status = InitApp();
   }

   /*
   ** At this point many flight apps use CFE_ES_WaitForStartupSync() to
   ** synchronize their startup timing with other apps. This is not
   ** needed for this simple app.
   */

   if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus))
   {
 
      /*
      ** This is just a an example loop. There are many ways to control the
      ** main loop execution flow.
      */
	  
	   CFE_ES_PerfLogExit(SLA_TLM_MAIN_PERF_ID);
      OS_TaskDelay(SLA_TLM_RUNLOOP_DELAY);
      CFE_ES_PerfLogEntry(SLA_TLM_MAIN_PERF_ID);

      ProcessCommands();

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("SLA_TLM Terminating, RunLoop status = 0x%08X\n", RunStatus);

   CFE_EVS_SendEvent(SLA_TLM_EXIT_ERR_EID, CFE_EVS_CRITICAL, "SLA_TLM Terminating,  RunLoop status = 0x%08X", RunStatus);

   CFE_ES_PerfLogExit(SLA_TLM_MAIN_PERF_ID);
   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of SLA_TLM_Main() */


/******************************************************************************
** Function: SLA_TLM_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean SLA_TLM_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (SLA_TLM_CMD_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "No operation command received for SLA_TLM version %d.%d",
                      SLA_TLM_MAJOR_VERSION,SLA_TLM_MINOR_VERSION);

   return TRUE;


} /* End SLA_TLM_NoOpCmd() */


/******************************************************************************
** Function: SLA_TLM_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean SLA_TLM_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);

   return TRUE;

} /* End SLA_TLM_ResetAppCmd() */


/******************************************************************************
** Function: SLA_TLM_SendHousekeepingPkt
**
*/
void SLA_TLM_SendHousekeepingPkt(void)
{

   // CFE_ES_WriteToSysLog("SLA_TLM_SendHKMsg Key: %x, Status: %x", 
   //           Sla_tlm.AttributionKey, Sla_tlm.PingStatus);

   /*
   ** CMDMGR Data
   */

   Sla_tlmHkPkt.ValidCmdCnt   = Sla_tlm.CmdMgr.ValidCmdCnt;
   Sla_tlmHkPkt.InvalidCmdCnt = Sla_tlm.CmdMgr.InvalidCmdCnt;

   Sla_tlmHkPkt.Key  = Sla_tlm.AttributionKey;
   Sla_tlmHkPkt.PingStatus = Sla_tlm.PingStatus;

   Sla_tlmHkPkt.RoundNum = Sla_tlm.RoundNum;
   Sla_tlmHkPkt.SequenceNum = Sla_tlm.SequenceNum;

   Sla_tlmHkPkt.CommTelemField1 = Sla_tlm.CommTelemField1;
   Sla_tlmHkPkt.CommTelemField2 = Sla_tlm.CommTelemField2;
   Sla_tlmHkPkt.CommTelemField3 = Sla_tlm.CommTelemField3;
   Sla_tlmHkPkt.CommTelemField4 = Sla_tlm.CommTelemField4;
   Sla_tlmHkPkt.CommTelemField5 = Sla_tlm.CommTelemField5;
   Sla_tlmHkPkt.CommTelemField6 = Sla_tlm.CommTelemField6;
   Sla_tlmHkPkt.CommTelemField7 = Sla_tlm.CommTelemField7;
   Sla_tlmHkPkt.CommTelemField8 = Sla_tlm.CommTelemField8;

   Sla_tlmHkPkt.LastAction       = 0;
   Sla_tlmHkPkt.LastActionStatus = 0;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Sla_tlmHkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &Sla_tlmHkPkt);

} /* End SLA_TLM_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
    int32 Status = CFE_SUCCESS;

    
    /*
    ** Initialize 'entity' objects
    */

    /*
    ** Initialize cFE interfaces 
    */

    CFE_SB_CreatePipe(&Sla_tlm.CmdPipe, SLA_TLM_CMD_PIPE_DEPTH, SLA_TLM_CMD_PIPE_NAME);
    CFE_SB_Subscribe(SLA_TLM_CMD_MID, Sla_tlm.CmdPipe);
    CFE_SB_Subscribe(SLA_TLM_SEND_HK_MID, Sla_tlm.CmdPipe);

    CFE_SB_Subscribe(COMM_SLA_TLM_MID, Sla_tlm.CmdPipe);
    CFE_SB_Subscribe(COMM_PING_RAW_MID, Sla_tlm.CmdPipe);
    CFE_SB_Subscribe(COMM_PAYLOAD_TELEM_MID, Sla_tlm.CmdPipe);
    CFE_SB_Subscribe(COMM_SLA_PING_MID, Sla_tlm.CmdPipe);

    /*
    ** Initialize App Framework Components 
    */

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, SLA_TLM_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, SLA_TLM_ResetAppCmd, 0);
 
    CFE_SB_InitMsg(&Sla_tlmHkPkt, SLA_TLM_TLM_HK_MID, SLA_TLM_TLM_HK_LEN, TRUE);
    
    Sla_tlm.AttributionKey = _ATTRIBUTION_KEY_;
   //  Sla_tlm.RoundNum = 0xAA;
   //  Sla_tlm.SequenceNum = 0xBBBB;
   //  Sla_tlm.PingStatus  = 0x44434241044434241;
   //  Sla_tlm.CommTelemField1 = 0xCCCCDDDD;
   //  Sla_tlm.CommTelemField2 = 0xEEEEFFFF;
    
    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(SLA_TLM_INIT_INFO_EID,
                               CFE_EVS_INFORMATION,
                               "SLA_TLM Initialized. Version %d.%d.%d.%d",
                               SLA_TLM_MAJOR_VERSION,
                               SLA_TLM_MINOR_VERSION,
                               SLA_TLM_REVISION,
                               SLA_TLM_MISSION_REV);

    return(Status);

} /* End of InitApp() */



/******************************************************************************
** Function: ProcessCommands
**
*/
static void ProcessCommands(void)
{

   int32           Status;
   CFE_SB_Msg_t   *CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;
   uint16          PktLen;
   void           *Message;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, Sla_tlm.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);
      PktLen = CFE_SB_GetTotalMsgLength(CmdMsgPtr);
      // CFE_ES_WriteToSysLog("SLA: ProcessCommands: MsgId: 0x%x, PktLen: %d", MsgId, PktLen); 

      switch (MsgId)
      {
         case SLA_TLM_CMD_MID:
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case SLA_TLM_SEND_HK_MID:
            SLA_TLM_SendHousekeepingPkt();
            break;

         case COMM_SLA_TLM_MID:
            for (int i = 0; i < 20; i++){
               CFE_EVS_SendEvent (SLA_TLM_INIT_INFO_EID, CFE_EVS_INFORMATION, "[%d]:%x",i,CmdMsgPtr->Byte[i]);
            }
            CFE_EVS_SendEvent(SLA_TLM_INIT_INFO_EID, CFE_EVS_INFORMATION,
                              "SLA_TLM Received attr key packet, MID = 0x%4X",MsgId);

            CFE_ES_WriteToSysLog("PktLen: %d, MsgSize: %d", PktLen, SLA_TLM_AttrKeyPkt_LEN);

            Message = (SLA_TLM_AttrKeyPkt *)CmdMsgPtr;
            Sla_tlm.AttributionKey = ((SLA_TLM_AttrKeyPkt *)Message)->Key;

            CFE_ES_WriteToSysLog("SLA: ProcessCommands: MsgId: 0x%x, Key: 0x%llx", MsgId, Sla_tlm.AttributionKey); 

            break;

         case COMM_PING_RAW_MID:
            CFE_EVS_SendEvent(SLA_TLM_INIT_INFO_EID, CFE_EVS_INFORMATION,
                              "SLA_TLM Received ping packet, MID = 0x%4X",MsgId);

            Message = (SLA_TLM_PingPkt *)CmdMsgPtr;
            Sla_tlm.PingStatus = ((SLA_TLM_PingPkt *)Message)->Status;

            CFE_ES_WriteToSysLog("SLA Ping Packet: Status = %llx", Sla_tlm.PingStatus); 

            break;

         case COMM_PAYLOAD_TELEM_MID:
            CFE_EVS_SendEvent(SLA_TLM_INIT_INFO_EID, CFE_EVS_INFORMATION,
                              "SLA_TLM Received comm telemetry packet, MID = 0x%4X, Size: %d",
                              MsgId, PktLen);

            Message =(SLA_COMM_Telem_Pkt*)CmdMsgPtr;
            Sla_tlm.CommTelemField1 = ((SLA_COMM_Telem_Pkt *)Message)->field1;
            Sla_tlm.CommTelemField2 = ((SLA_COMM_Telem_Pkt *)Message)->field2;
            Sla_tlm.CommTelemField3 = ((SLA_COMM_Telem_Pkt *)Message)->field3;
            Sla_tlm.CommTelemField4 = ((SLA_COMM_Telem_Pkt *)Message)->field4;
            Sla_tlm.CommTelemField5 = ((SLA_COMM_Telem_Pkt *)Message)->field5;
            Sla_tlm.CommTelemField6 = ((SLA_COMM_Telem_Pkt *)Message)->field6;
            Sla_tlm.CommTelemField7 = ((SLA_COMM_Telem_Pkt *)Message)->field7;
            Sla_tlm.CommTelemField8 = ((SLA_COMM_Telem_Pkt *)Message)->field8;

            CFE_ES_WriteToSysLog("Recieved comm payload telemetry ");

            break;

         case COMM_SLA_PING_MID:
            CFE_EVS_SendEvent(SLA_TLM_INIT_INFO_EID, CFE_EVS_INFORMATION,
                              "SLA_TLM Received ping status packet, MID = 0x%4X",MsgId);

            Message = (SLA_TLM_CommPingStatus *)CmdMsgPtr;
            Sla_tlm.PingStatus = ((SLA_TLM_CommPingStatus *)Message)->Status;

            CFE_ES_WriteToSysLog("SLA Comm Ping Packet: Status = %llx", Sla_tlm.PingStatus); 

            break;

         default:
            CFE_EVS_SendEvent(SLA_TLM_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
