/*
** Purpose: Define the SBN-lite application. This is a simple UDP-based
** single peer application for networking two instances of cfs. The packet
** table defines packets that are subscribed to on the SB and transmitted
** over UDP.  Any packet received on the UDP socket is validated and sent
** onto the SB for that processor. 
**
**
*/

/*
** Includes
*/

#include <string.h>
// #include <math.h>
#include "sbn_lite_app.h"
#include "cfs_utils.h"
#include "cf_msgids.h"

/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);
static void SendHousekeepingPkt(SBN_LITE_HkPkt *HkPkt);
// static void SendPeriodicTransferCmd(void);

/*
** Global Data
*/

SBN_LITE_Class  SbnLite;


/* Convenience macros */
#define  CMDMGR_OBJ  (&(SbnLite.CmdMgr))
#define  TBLMGR_OBJ  (&(SbnLite.TblMgr))
#define  SBNTBL_OBJ  (&(SbnLite.SbnTbl))
#define  SBNMGR_OBJ  (&(SbnLite.SbnMgr))


/******************************************************************************
** Function: SBN_LITE_AppMain
**
*/
void SBN_LITE_AppMain(void)
{

   int32  Status    = CFE_SEVERITY_ERROR;
   uint32 RunStatus = CFE_ES_APP_ERROR;
   uint32 StartupCnt;
   uint16 NumPktsOutput;

   Status = CFE_ES_RegisterApp();
   CFE_EVS_Register(NULL,0,0);

   /*
   ** Perform application specific initialization
   */
   if (Status == CFE_SUCCESS) {
      
      CFE_EVS_SendEvent(SBN_LITE_INIT_DEBUG_EID, SBN_LITE_INIT_EVS_TYPE, "SBN_LITE: About to call init\n");
      Status = InitApp();
   
   }

   if (Status == CFE_SUCCESS) {
      
      RunStatus = CFE_ES_APP_RUN;
   
   }
   
   /*
   ** Main process loop
   */
   
   CFE_EVS_SendEvent(SBN_LITE_INIT_DEBUG_EID, SBN_LITE_INIT_EVS_TYPE, "SBN_LITE: About to enter loop\n");
   StartupCnt = 0;
   while (CFE_ES_RunLoop(&RunStatus)) {
   
      /* Use a short delay during startup to avoid event message pipe overflow */
      if (StartupCnt < 200) { 
         OS_TaskDelay(20);
         ++StartupCnt;
      }
      else {
         OS_TaskDelay(SbnLite.RunLoopDelay);
      }

      NumPktsOutput = SBNMGR_OutputPackets();

      SBNMGR_ReadPackets(SBN_LITE_RUNLOOP_MSG_READ);
      
      CFE_EVS_SendEvent(SBN_LITE_DEMO_EID, CFE_EVS_DEBUG, "Output %d telemetry packets", NumPktsOutput);

      ProcessCommands();

// #ifdef PERIODIC_TRANSFER
//       if (SbnLite.ExecCnt % (PERIODIC_TRANSFER_RATE_MS/SbnLite.RunLoopDelay) == 0) {
//          SendPeriodicTransferCmd();
//       }
// #endif
   SbnLite.ExecCnt++;

   } /* End CFE_ES_RunLoop */


   /* Write to system log in case events not working */

   CFE_ES_WriteToSysLog("SBN_LITE App terminating, err = 0x%08X\n", Status);

   CFE_EVS_SendEvent(SBN_LITE_APP_EXIT_EID, CFE_EVS_CRITICAL, "SBN_LITE App: terminating, err = 0x%08X", Status);

   CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of KIT_TO_AppMain() */


/******************************************************************************
** Function: SBN_LITE_NoOpCmd
**
*/

boolean SBN_LITE_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (SBN_LITE_APP_NOOP_EID, CFE_EVS_INFORMATION,
                      "Software Bus Network Lite (SBN-Lite) version %d.%d.%d received a no operation command",
                      SBN_LITE_MAJOR_VER,SBN_LITE_MINOR_VER,SBN_LITE_LOCAL_REV);

   return TRUE;


} /* End SBN_LITE_NoOpCmd() */


/******************************************************************************
** Function: SBN_LITE_ResetAppCmd
**
*/

boolean SBN_LITE_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);

   SBNMGR_ResetStatus();

   return TRUE;

} /* End SBN_LITE_ResetAppCmd() */


/******************************************************************************
** Function: SBN_LITE_SetRunLoopDelayCmd
**
*/
boolean SBN_LITE_SetRunLoopDelayCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   const SBN_LITE_SetRunLoopDelayCmdParam *CmdParam = (const SBN_LITE_SetRunLoopDelayCmdParam *) MsgPtr;
   SBN_LITE_Class *SbnLitePtr = (SBN_LITE_Class *)ObjDataPtr;
   boolean RetStatus = FALSE;
   
   if ((CmdParam->RunLoopDelay >= SBN_LITE_MIN_RUN_LOOP_DELAY_MS) &&
       (CmdParam->RunLoopDelay <= SBN_LITE_MAX_RUN_LOOP_DELAY_MS)) {
   
      CFE_EVS_SendEvent(SBN_LITE_SET_RUN_LOOP_DELAY_EID, CFE_EVS_INFORMATION,
                        "SBN-Lite Run loop delay changed from %d to %d", 
                        SbnLitePtr->RunLoopDelay, CmdParam->RunLoopDelay);
   
      SbnLitePtr->RunLoopDelay = CmdParam->RunLoopDelay;
      
      SBNMGR_InitStats(SbnLitePtr->RunLoopDelay,SBNMGR_STATS_RECONFIG_INIT_MS);

      RetStatus = TRUE;
   
   }   
   else {
      
      CFE_EVS_SendEvent(SBN_LITE_INVALID_RUN_LOOP_DELAY_EID, CFE_EVS_ERROR,
                        "Invalid commanded run loop delay of %d ms. Valid inclusive range: [%d,%d] ms", 
                        CmdParam->RunLoopDelay,SBN_LITE_MIN_RUN_LOOP_DELAY_MS,SBN_LITE_MAX_RUN_LOOP_DELAY_MS);
      
   }
   
   return RetStatus;
   
} /* End SBN_LITE_SetRunLoopDelayCmd() */


/******************************************************************************
** Function: SendHousekeepingPkt
**
*/
static void SendHousekeepingPkt(SBN_LITE_HkPkt *HkPkt)
{

   /*
   ** SBN_LITE Data
   */

   HkPkt->ValidCmdCnt   = SbnLite.CmdMgr.ValidCmdCnt;
   HkPkt->InvalidCmdCnt = SbnLite.CmdMgr.InvalidCmdCnt;

   HkPkt->RunLoopDelay  = SbnLite.RunLoopDelay;

   /*
   ** TBLMGR Data
   */

   HkPkt->PktTblLastLoadStatus  = SbnLite.SbnTbl.LastLoadStatus;
   HkPkt->PktTblAttrErrCnt      = SbnLite.SbnTbl.AttrErrCnt;

   /*
   ** SBNMGR Data
   ** - At a minimum all pktmgr variables effected by a reset must be included
   ** - Some of these may be more diagnostic but not enough to warrant a
   **   separate diagnostic. Also easier for the user not to have to command it.
   */

   HkPkt->StatsValid  = (SbnLite.SbnMgr.Stats.State == SBNMGR_STATS_VALID);

   // JRL - I know this will truncate the final number but had an issue forcing
   // the round symbol into the build
   HkPkt->TxPktsPerSec  = (uint16)(SbnLite.SbnMgr.Stats.TxAvgPktsPerSec);
   HkPkt->TxBytesPerSec = (uint32)(SbnLite.SbnMgr.Stats.TxAvgBytesPerSec);

   HkPkt->RxPktCnt = SbnLite.SbnMgr.RxPktCnt;
   HkPkt->RxPktErrCnt = SbnLite.SbnMgr.RxPktErrCnt;

   HkPkt->TxSockId = (uint16)SbnLite.SbnMgr.TxSockId;
   strncpy(HkPkt->TxDestIp, SbnLite.SbnMgr.TxDestIp, SBNMGR_IP_STR_LEN);

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) HkPkt);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) HkPkt);

} /* End SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
   int32 Status = CFE_SUCCESS;

   CFE_EVS_SendEvent(SBN_LITE_INIT_DEBUG_EID, SBN_LITE_INIT_EVS_TYPE, "SBN_LITE_InitApp() Entry\n");

   SbnLite.RunLoopDelay = SBN_LITE_RUN_LOOP_DELAY_MS;
   SbnLite.ExecCnt = 0;

   /*
   ** Initialize 'entity' objects
   */

   SBNTBL_Constructor(SBNTBL_OBJ, SBNMGR_GetTblPtr, SBNMGR_LoadTbl, SBNMGR_LoadTblEntry);
   SBNMGR_Constructor(SBNMGR_OBJ, SBNMGR_PIPE_NAME, SBNMGR_PIPE_DEPTH);

   /*
   ** Initialize application managers
   */

   CFE_SB_CreatePipe(&SbnLite.CmdPipe, SBN_LITE_CMD_PIPE_DEPTH, SBN_LITE_CMD_PIPE_NAME);
   CFE_SB_Subscribe(SBN_LITE_CMD_MID, SbnLite.CmdPipe);
   CFE_SB_Subscribe(SBN_LITE_SEND_HK_MID, SbnLite.CmdPipe);

   CFE_EVS_SendEvent(SBN_LITE_INIT_DEBUG_EID, SBN_LITE_INIT_EVS_TYPE, "SBN_LITE_InitApp() Before CMDMGR calls\n");
   CMDMGR_Constructor(CMDMGR_OBJ);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,            NULL,       SBN_LITE_NoOpCmd,            0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,           NULL,       SBN_LITE_ResetAppCmd,        0);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_PKT_TBL_LOAD_CMD_FC,    TBLMGR_OBJ, TBLMGR_LoadTblCmd,         TBLMGR_LOAD_TBL_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_PKT_TBL_DUMP_CMD_FC,    TBLMGR_OBJ, TBLMGR_DumpTblCmd,         TBLMGR_DUMP_TBL_CMD_DATA_LEN);

   CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_ADD_PKT_CMD_FC,          SBNMGR_OBJ, SBNMGR_AddPktCmd,          PKKTMGR_ADD_PKT_CMD_DATA_LEN);
   // CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_ENABLE_OUTPUT_CMD_FC,    SBNMGR_OBJ, SBNMGR_EnableOutputCmd,    PKKTMGR_ENABLE_OUTPUT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_REMOVE_ALL_PKTS_CMD_FC,  SBNMGR_OBJ, SBNMGR_RemoveAllPktsCmd,   0);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_REMOVE_PKT_CMD_FC,       SBNMGR_OBJ, SBNMGR_RemovePktCmd,       PKKTMGR_REMOVE_PKT_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_SEND_PKT_TBL_TLM_CMD_FC, SBNMGR_OBJ, SBNMGR_SendPktTblTlmCmd,   PKKTMGR_SEND_PKT_TBL_TLM_CMD_DATA_LEN);
   CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_UPDATE_FILTER_CMD_FC,    SBNMGR_OBJ, SBNMGR_UpdateFilterCmd,    PKKTMGR_UPDATE_FILTER_CMD_DATA_LEN);
   
   CMDMGR_RegisterFunc(CMDMGR_OBJ, SBN_LITE_SET_RUN_LOOP_DELAY_CMD_FC, &SbnLite,  SBN_LITE_SetRunLoopDelayCmd, SBN_LITE_SET_RUN_LOOP_DELAY_CMD_DATA_LEN);

   CFE_EVS_SendEvent(SBN_LITE_INIT_DEBUG_EID, SBN_LITE_INIT_EVS_TYPE, "SBN_LITE_InitApp() Before TBLMGR calls\n");
   TBLMGR_Constructor(TBLMGR_OBJ);
   TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, SBNTBL_LoadCmd, SBNTBL_DumpCmd, SBN_LITE_DEF_PKTTBL_FILE_NAME);

   CFE_SB_InitMsg(&SbnLite.HkPkt, SBN_LITE_HK_TLM_MID, SBN_LITE_TLM_HK_LEN, TRUE);
// #ifdef PERIODIC_TRANSFER
//    CFE_SB_InitMsg(&SbnLite.CF_Playback_cmd, CF_CMD_MID,CF_PLAYBACK_CMD_LEN,TRUE);
//    CFE_SB_SetCmdCode((CFE_SB_MsgPtr_t)&SbnLite.CF_Playback_cmd, CF_PLAYBACK_FILE_CC);
//    CFE_SB_SetUserDataLength((CFE_SB_MsgPtr_t)&SbnLite.CF_Playback_cmd, CF_PLAYBACK_CMD_LEN-CFE_SB_CMD_HDR_SIZE);
// #endif

   /*
   ** Application startup event message
   */

   Status = CFE_EVS_SendEvent(SBN_LITE_APP_INIT_EID, CFE_EVS_INFORMATION,
                              "SBN_LITE Initialized. Version %d.%d.%d",
                              SBN_LITE_MAJOR_VER, SBN_LITE_MINOR_VER, SBN_LITE_LOCAL_REV);

   return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCommands
**
*/
static void ProcessCommands(void)
{

   int32           Status;
   CFE_SB_Msg_t*   CmdMsgPtr;
   CFE_SB_MsgId_t  MsgId;

   Status = CFE_SB_RcvMsg(&CmdMsgPtr, SbnLite.CmdPipe, CFE_SB_POLL);

   if (Status == CFE_SUCCESS)
   {

      MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

      switch (MsgId)
      {
         case SBN_LITE_CMD_MID:
            CFE_EVS_SendEvent(SBN_LITE_DEMO_EID, CFE_EVS_DEBUG, "Processing command function code %d", CFE_SB_GetCmdCode(CmdMsgPtr));
            CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
            break;

         case SBN_LITE_SEND_HK_MID:
            CFE_EVS_SendEvent(SBN_LITE_DEMO_EID, CFE_EVS_DEBUG, "Sending housekeeping packet");
            SendHousekeepingPkt(&SbnLite.HkPkt);
            break;

         default:
            CFE_EVS_SendEvent(SBN_LITE_APP_INVALID_MID_EID, CFE_EVS_ERROR,
                              "Received invalid command packet,MID = 0x%4X",MsgId);

            break;

      } /* End Msgid switch */

   } /* End if SB received a packet */

} /* End ProcessCommands() */


