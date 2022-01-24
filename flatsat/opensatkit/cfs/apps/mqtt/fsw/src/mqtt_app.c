/*
** Purpose: Implement the MQTT application
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

/*
** Includes
*/

#include <string.h>
#include "mqtt_app.h"


/***********************/
/** Macro Definitions **/
/***********************/

/* Convenience macros */
#define  INITBL_OBJ      (&(Mqtt.IniTbl))
#define  CMDMGR_OBJ      (&(Mqtt.CmdMgr))
#define  TBLMGR_OBJ      (&(Mqtt.TblMgr))    
#define  CHILDMGR_OBJ    (&(Mqtt.ChildMgr))
#define  MQ2SB_OBJ       (&(Mqtt.Mq2Sb))
#define  SB2MQ_OBJ       (&(Mqtt.Sb2Mq))
#define  MQTT_MSG_OBJ    (&(Mqtt.MqttMsg))
#define  MQTT_RECV_CLIENT_OBJ (&(Mqtt.MqttRecvClient))
#define  MQTT_PUB_CLIENT_OBJ (&(Mqtt.MqttPubClient))

/*******************************/
/** Local Function Prototypes **/
/*******************************/

static int32 InitApp(uint32* AppMainPerfId);
static void ProcessCommands(void);
static boolean ChildTaskCallback(CHILDMGR_Class* ChildMgr);

/**********************/
/** File Global Data **/
/**********************/

/* 
** Must match DECLARE ENUM() declaration in app_cfg.h
** Defines "static INILIB_CfgEnum IniCfgEnum"
*/
DEFINE_ENUM(Config,APP_CONFIG)  

// static CFE_EVS_BinFilter_t  EventFilters[] =
// {  
//    /* Event ID                 Mask */
//    {MQTT_CLIENT_YIELD_ERR_EID, CFE_EVS_FIRST_4_STOP}

// };

/*****************/
/** Global Data **/
/*****************/

MQTT_Class   Mqtt;
CFE_SB_MsgId_t CmdMid;
CFE_SB_MsgId_t SendHkMid;

/******************************************************************************
** Function: MQTT_AppMain
**
*/
void MQTT_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;
   uint32 AppMainPerfId;
   CmdMid    = CFE_SB_INVALID_MSG_ID;
   SendHkMid = CFE_SB_INVALID_MSG_ID;

   Status = CFE_ES_RegisterApp();
   
   if (Status == CFE_SUCCESS) {
   
      // CFE_EVS_Register(EventFilters, sizeof(EventFilters)/sizeof(CFE_EVS_BinFilter_t),
      //                  CFE_EVS_BINARY_FILTER);

      CFE_EVS_Register(NULL,0,0);

      Status = InitApp(&AppMainPerfId); /* Performs initial CFE_ES_PerfLogEntry() call */
   
      if (Status == CFE_SUCCESS) {

         RunStatus = CFE_ES_APP_RUN;

         // AppMainPerfId = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_MAIN_PERF_ID);
         CmdMid    = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID);
         SendHkMid = (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID);

      }
      
   } /* End if RegisterApp() success */
   
   /*
   ** Main process loop
   */
   while (CFE_ES_RunLoop(&RunStatus)) {

      CFE_ES_PerfLogEntry(AppMainPerfId);
      SB2MQ_OutputPackets();
      ProcessCommands();
      OS_TaskDelay(Mqtt.RunLoopDelay);
      CFE_ES_PerfLogExit(AppMainPerfId);

   } /* End CFE_ES_RunLoop */

   CFE_ES_WriteToSysLog("MQTT App terminating, err = 0x%08X\n", Status);   /* Use SysLog, events may not be working */

   CFE_EVS_SendEvent(MQTT_EXIT_EID, CFE_EVS_CRITICAL, "MQTT App terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of MQTT_AppMain() */

/******************************************************************************
** Function: ProcessCommands
**
*/
static void ProcessCommands(void)
{

   int32           Status;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, Mqtt.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS) {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);
      
      if (MsgId == CmdMid) {
         CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
      } 
      else if (MsgId == SendHkMid) {

         MQTT_SendHousekeepingPkt();
      }
      else {
            CFE_EVS_SendEvent(MQTT_INVALID_MID_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);
      }
      // // Subscribe to MQTT Topics if needed
      // if (!MQTT_CLIENT_OBJ->Subscribed && MQTT_CLIENT_OBJ->Connected) {
      //       CFE_EVS_SendEvent(MQTT_CLIENT_SUBSCRIBE_EID, CFE_EVS_INFORMATION,
      //                   "Connected, but not subscribed.  Calling subscribe.");
      //       SB2MQT_ClientSubscribe();
      //       MQTT_CLIENT_OBJ->Subscribed = true;
      // }

      

   } /* End if SB received a packet */
   else {
   
      Status = CFE_ES_APP_ERROR;
   
   } /* End if SB failure */

}
/******************************************************************************
** Function: MQTT_NoOpCmd
**
*/

boolean MQTT_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (MQTT_NOOP_EID, CFE_EVS_INFORMATION,
                      "No operation command received for MQTT App version %d.%d.%d",
                      MQTT_MAJOR_VER, MQTT_MINOR_VER, MQTT_PLATFORM_REV);

   return TRUE;


} /* End MQTT_NoOpCmd() */


/******************************************************************************
** Function: MQTT_ResetAppCmd
**
*/

boolean MQTT_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);
   CHILDMGR_ResetStatus(CHILDMGR_OBJ);
   
   MQ2SB_ResetStatus();
	  
   return TRUE;

} /* End MQTT_ResetAppCmd() */


/******************************************************************************
** Function: MQTT_SetRunLoopDelayCmd
**
*/
boolean MQTT_SetRunLoopDelayCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const MQTT_SetRunLoopDelayCmdParam *CmdParam = (const MQTT_SetRunLoopDelayCmdParam *) MsgPtr;
   boolean RetStatus = FALSE;
   
   if ((CmdParam->RunLoopDelay >= MQTT_MIN_RUN_LOOP_DELAY_MS) &&
       (CmdParam->RunLoopDelay <= MQTT_MAX_RUN_LOOP_DELAY_MS)) {
   
      CFE_EVS_SendEvent(MQTT_SET_RUNLOOP_DELAY_EID, CFE_EVS_INFORMATION,
                        "MQTT App Run loop delay changed from %d to %d", 
                        Mqtt.RunLoopDelay, CmdParam->RunLoopDelay);
   
      Mqtt.RunLoopDelay = CmdParam->RunLoopDelay;
      
      // SBNMGR_InitStats(SbnLitePtr->RunLoopDelay,SBNMGR_STATS_RECONFIG_INIT_MS);

      RetStatus = TRUE;
   
   }   
   else {
      
      CFE_EVS_SendEvent(MQTT_INVALID_RUNLOOP_DELAY_EID, CFE_EVS_ERROR,
                        "Invalid commanded run loop delay of %d ms. Valid inclusive range: [%d,%d] ms", 
                        CmdParam->RunLoopDelay,MQTT_MIN_RUN_LOOP_DELAY_MS,MQTT_MAX_RUN_LOOP_DELAY_MS);
      
   }
   
   return RetStatus;
   
} /* End MQTT_SetRunLoopDelayCmd() */

/******************************************************************************
** Function: MQTT_SendHousekeepingPkt
**
*/
void MQTT_SendHousekeepingPkt(void)
{
   
   Mqtt.HkPkt.ValidCmdCnt    = Mqtt.CmdMgr.ValidCmdCnt;
   Mqtt.HkPkt.InvalidCmdCnt  = Mqtt.CmdMgr.InvalidCmdCnt;

   Mqtt.HkPkt.ChildValidCmdCnt    = Mqtt.ChildMgr.ValidCmdCnt;
   Mqtt.HkPkt.ChildInvalidCmdCnt  = Mqtt.ChildMgr.InvalidCmdCnt;

   Mqtt.HkPkt.RecvConnected = MQTT_RECV_CLIENT_OBJ->Connected;
   Mqtt.HkPkt.RecvSubscriptionCount = MQTT_RECV_CLIENT_OBJ->SubscriptionCount;
   Mqtt.HkPkt.RecvReconnectCount = MQTT_RECV_CLIENT_OBJ->ReconnectCount;
   Mqtt.HkPkt.RecvMsgCnt = MQTT_RECV_CLIENT_OBJ->MsgRecvCnt;
   Mqtt.HkPkt.PubConnected = MQTT_PUB_CLIENT_OBJ->Connected;
   Mqtt.HkPkt.PubReconnectCount = MQTT_PUB_CLIENT_OBJ->ReconnectCount;
   Mqtt.HkPkt.PubMsgCnt = MQTT_PUB_CLIENT_OBJ->MsgPubCnt;


   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Mqtt.HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &Mqtt.HkPkt);

} /* End MQTT_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(uint32* AppMainPerfId)
{

   int32 Status = OSK_C_FW_CFS_ERROR;
   
   CHILDMGR_TaskInit ChildTaskInit;
   
   /*
   ** Initialize objects 
   */
   if (INITBL_Constructor(&Mqtt.IniTbl, MQTT_INI_FILENAME, &IniCfgEnum)) {
   
      *AppMainPerfId = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_MAIN_PERF_ID);
      CFE_ES_PerfLogEntry(*AppMainPerfId);
      Mqtt.RunLoopDelay = INITBL_GetIntConfig(INITBL_OBJ, CFG_APP_RUNLOOP_DELAY); // Set initial runloop delay from INI table
      Mqtt.ChildTaskDelay = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_TASK_DELAY); // Set initial child task delay from INI table

      MQTT_LOG_INFO("Starting MQTT Recieve Client");
            
      MQTT_CLIENT_Constructor(MQTT_RECV_CLIENT_OBJ,
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_ADDRESS), 
                              INITBL_GetIntConfig(INITBL_OBJ, CFG_SERVER_PORT), 
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_CLIENT_RECV_NAME), 
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_USERNAME),
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_PASSWORD),
                              INITBL_GetIntConfig(INITBL_OBJ, CFG_CLIENT_RECV_KEEPALIVE),
                              INITBL_GetIntConfig(INITBL_OBJ, CFG_CLIENT_YIELD_INTERVAL),
                              false);

      MQTT_LOG_INFO("Starting MQTT Pub Client");

      MQTT_CLIENT_Constructor(MQTT_PUB_CLIENT_OBJ,
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_ADDRESS), 
                              INITBL_GetIntConfig(INITBL_OBJ, CFG_SERVER_PORT), 
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_CLIENT_PUB_NAME), 
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_USERNAME),
                              INITBL_GetStrConfig(INITBL_OBJ, CFG_SERVER_PASSWORD),
                              INITBL_GetIntConfig(INITBL_OBJ, CFG_CLIENT_PUB_KEEPALIVE),
                              INITBL_GetIntConfig(INITBL_OBJ, CFG_CLIENT_PUB_TIMEOUT_INTERVAL),
                              false);
       
      MQTT_LOG_INFO("Setting Child Task");
      /* Constructor sends error events */    
      ChildTaskInit.TaskName  = INITBL_GetStrConfig(INITBL_OBJ, CFG_CHILD_NAME);
      ChildTaskInit.StackSize = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_STACK_SIZE);
      ChildTaskInit.Priority  = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_PRIORITY);
      ChildTaskInit.PerfId    = INITBL_GetIntConfig(INITBL_OBJ, CFG_CHILD_TASK_PERF_ID);
      Status = CHILDMGR_Constructor(CHILDMGR_OBJ, ChildMgr_TaskMainCallback,
                                    ChildTaskCallback, 
                                    &ChildTaskInit); 
      
      MQTT_LOG_INFO("MQTT Child Task Init Status: %d", Status);
   
   } /* End if INITBL Constructed */
   if (Status == CFE_SUCCESS) {

      MQ2SB_Constructor(MQ2SB_OBJ, MQTT_RECV_CLIENT_OBJ,  &Mqtt.RunLoopDelay,
                        MQ2SB_GetTblPtr, MQ2SB_LoadTbl, MQ2SB_LoadTblEntry);

      SB2MQ_Constructor(SB2MQ_OBJ, MQTT_PUB_CLIENT_OBJ, &Mqtt.RunLoopDelay,
                        SB2MQ_GetTblPtr, SB2MQ_LoadTbl, SB2MQ_LoadTblEntry,
                        INITBL_GetStrConfig(INITBL_OBJ, SB2MQ_CFG_PIPE_NAME),
                        INITBL_GetIntConfig(INITBL_OBJ, SB2MQ_CFG_PIPE_DEPTH));


      MQTT_LOG_INFO("Finished MQTT Client Constructors");
      // JRL - SB2MQT_ClientSubscribe() would get called prior to connection on LEON3
      // Moved to main run loop
      // if (MQ2SB_OBJ->CfeTbl.Status == CFE_SUCCESS) {
      
      //    SB2MQT_ClientSubscribe();
         
      // }

      /*
      ** Initialize app level interfaces
      */
      
      CFE_SB_CreatePipe(&Mqtt.CmdPipe, INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_PIPE_DEPTH), INITBL_GetStrConfig(INITBL_OBJ, CFG_CMD_PIPE_NAME));  

      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_CMD_MID), Mqtt.CmdPipe);
      CFE_SB_Subscribe((CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_SEND_HK_MID), Mqtt.CmdPipe);


      CMDMGR_Constructor(CMDMGR_OBJ);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,   NULL, MQTT_NoOpCmd,     0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,  NULL, MQTT_ResetAppCmd, 0);
      CMDMGR_RegisterFunc(CMDMGR_OBJ, MQTT_SET_RUN_LOOP_DELAY_CMD_FC,  NULL, MQTT_SetRunLoopDelayCmd, 0);
      // CMDMGR_RegisterFunc(CMDMGR_OBJ, MQTT_SB2MQ_REMOVE_ALL_PKTS_CMD_FC,  NULL, SB2MQ_RemoveAllPktsCmd,   0);
      // CMDMGR_RegisterFunc(CMDMGR_OBJ, MQTT_SB2MQ_REMOVE_PKT_CMD_FC,       NULL, SB2MQ_RemovePktCmd,       SB2MQ_REMOVE_PKT_CMD_DATA_LEN);
      // CMDMGR_RegisterFunc(CMDMGR_OBJ, MQTT_SB2MQ_SEND_PKT_TBL_TLM_CMD_FC, NULL, SB2MQ_SendPktTblTlmCmd,   SB2MQ_SEND_PKT_TBL_TLM_CMD_DATA_LEN);
      // CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_UPDATE_FILTER_CMD_FC,    NULL, SBNMGR_UpdateFilterCmd,    PKKTMGR_UPDATE_FILTER_CMD_DATA_LEN);
      //Child command codin idiom
      //CMDMGR_RegisterFunc(CMDMGR_OBJ, DIR_CREATE_CMD_FC,          DIR_OBJ, CHILDMGR_InvokeChildCmd, DIR_CREATE_CMD_DATA_LEN);

      CFE_SB_InitMsg(&Mqtt.HkPkt, (CFE_SB_MsgId_t)INITBL_GetIntConfig(INITBL_OBJ, CFG_HK_TLM_MID), 
                     MQTT_TLM_HK_PKT_LEN, TRUE);

      TBLMGR_Constructor(TBLMGR_OBJ);
      TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, MQ2SB_LoadCmd, MQ2SB_DumpCmd, INITBL_GetStrConfig(INITBL_OBJ, MQ2SB_CFG_TBL_DEF_FILENAME));
      TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, SB2MQ_LoadCmd, SB2MQ_DumpCmd, INITBL_GetStrConfig(INITBL_OBJ, SB2MQ_CFG_TBL_DEF_FILENAME));
   
      /*
      ** Application startup event message
      */
      CFE_EVS_SendEvent(MQTT_INIT_APP_EID, CFE_EVS_INFORMATION,
                        "MQTT App Initialized. Version %d.%d.%d",
                        MQTT_MAJOR_VER, MQTT_MINOR_VER, MQTT_PLATFORM_REV);
                        
   } /* End if CHILDMGR constructed */
   
   return(Status);

} /* End of InitApp() */



/******************************************************************************
** Function: ChildTaskCallback
**
*/
static boolean ChildTaskCallback(CHILDMGR_Class* ChildMgr)
{
   int RetCode;
   boolean RetStatus = FALSE;
   boolean Connected = FALSE;

   if (MQ2SB_OBJ->LastLoadStatus == TBLMGR_STATUS_VALID ) {
      // CFE_EVS_SendEvent(MQTT_CHILD_TASK_DEBUG_EID, CFE_EVS_INFORMATION,
      //                   "Entering MQTT Recv Child Task Loop");
      Connected = MQTT_CLIENT_Connected(MQTT_RECV_CLIENT_OBJ);
      if(!Connected) {
         CFE_EVS_SendEvent(MQTT_CHILD_TASK_NOT_CONNECTED_EID, CFE_EVS_ERROR,
            "MQTT Recv Client Not Connected, Reconnecting");
         MQTT_RECV_CLIENT_OBJ->Subscribed = false;
         MQTT_CLIENT_Disconnect(&Mqtt.MqttRecvClient);
         RetCode = MQTT_CLIENT_NetworkInitAndConnect(&Mqtt.MqttRecvClient);
         if (RetCode == SUCCESS) {
            Connected = MQTT_CLIENT_Connect(&Mqtt.MqttRecvClient);
            MQTT_LOG_INFO("MQTT Connection Result. Connected: %s", Connected ? "true" : "false");
            MQTT_RECV_CLIENT_OBJ->ReconnectCount++;
         } else {
            MQTT_LOG_ERROR("MQTT Network Connection Failed");
         }
         Connected = MQTT_CLIENT_Connected(MQTT_RECV_CLIENT_OBJ);
      }

      if (!MQTT_RECV_CLIENT_OBJ->Subscribed && Connected) {
         CFE_EVS_SendEvent(MQTT_CHILD_TASK_SUB_ERROR_EID, CFE_EVS_INFORMATION,
                     "Connected, but not subscribed.  Calling subscribe.");
         MQ2SB_ClientSubscribe();
         MQTT_RECV_CLIENT_OBJ->Subscribed = true;
      }

      // If we're not connected try to connect
      if (Connected) {
         // Need to call task delay after yield to ensure non-blocking
         RetStatus = MQTT_CLIENT_Yield(&Mqtt.MqttRecvClient);
         if(!RetStatus) {
            CFE_EVS_SendEvent(MQTT_CHILD_TASK_YIELD_EID, CFE_EVS_DEBUG,
               "MQTT Recv Client Yield Error.");
         }
         OS_TaskDelay(Mqtt.ChildTaskDelay);
      }
   }
   else {

      OS_TaskDelay(Mqtt.ChildTaskDelay);
   
   }

   return TRUE;
   
} /* End ChildTaskCallback() */



