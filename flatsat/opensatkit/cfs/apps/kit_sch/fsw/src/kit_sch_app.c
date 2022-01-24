/* 
** Purpose: Implement OPenSat Kit Scheduler application.
**
** Notes:
**   1. This is non-flight code so an attempt has been made to balance keeping
**      it simple while making it robust. Limiting the number of configuration
**      parameters and integration items (message IDs, perf IDs, etc) was
**      also taken into consideration.
**   2. Event message filters are not used since this is for test environments.
**      This may be reconsidered if event flooding ever becomes a problem.
**   3. Performance traces are not included.
**   4. Most functions are global to assist in unit testing
**   5. Functions I removed from original that need to be thought thru
**        SCH_ValidateMessageData()
**        SCH_ValidateScheduleData()
**        SCH_ProcessCommands()
**        SCH_TblInit()
**        InitEventFilters()
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
** Includes
*/

#include "kit_sch_app.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCmdPkt(CFE_SB_MsgPtr_t CmdMsgPtr);

/*
** Global Data
*/

KIT_SCH_Class  KitSch;

KIT_SCH_HkPkt  KitSchHk;

/* Convenience macros */
#define  CMDMGR_OBJ    (&(KitSch.CmdMgr)) 
#define  TBLMGR_OBJ    (&(KitSch.TblMgr))
#define  SCHTBL_OBJ    (&(KitSch.SchTbl))
#define  MSGTBL_OBJ    (&(KitSch.MsgTbl))
#define  SCHEDULER_OBJ (&(KitSch.Scheduler))

/******************************************************************************
** Function: KIT_SCH_Main
**
*/
void KIT_SCH_AppMain(void)
{
	
    int32  Status    = CFE_SEVERITY_ERROR;
    uint32 RunStatus = CFE_ES_APP_ERROR;
    CFE_SB_Msg_t* CmdMsgPtr;

    /*
    ** Register application
    */
    
    Status = CFE_ES_RegisterApp();

    CFE_EVS_Register(NULL,0,CFE_EVS_NO_FILTER);

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
    ** needed.
    */
    if (Status == CFE_SUCCESS)
    {

        /*
        ** Flight version includes a call to CFE_ES_WaitForStartupSync(). Since
        ** this will be used in a dynamic test environment the idea is let the
        ** default scheduler start and add applications as needed from the user
        ** console as opposed to trying to synchronize everything in the embedded
        ** system.
        */

        Status = SCHEDULER_StartTimers();

    } /* End if App initialized successfully */

    if (Status == CFE_SUCCESS) RunStatus = CFE_ES_APP_RUN;

    /*
    ** Main process loop
    */
    CFE_EVS_SendEvent(KIT_SCH_DEBUG_EID, CFE_EVS_DEBUG,"KIT_SCH: About to enter loop\n");
    while (CFE_ES_RunLoop(&RunStatus))
    {

       if (!SCHEDULER_Execute())
       {
            RunStatus = CFE_ES_APP_ERROR;
       }

       Status = CFE_SB_RcvMsg(&CmdMsgPtr, KitSch.CmdPipe, CFE_SB_POLL);
        
       if (Status == CFE_SUCCESS)
       {
          ProcessCmdPkt(CmdMsgPtr);
       }

    } /* End CFE_ES_RunLoop */


    /* Write to system log in case events not working */

    CFE_ES_WriteToSysLog("KIT_SCH App terminating, err = 0x%08X\n", Status);

    CFE_EVS_SendEvent(KIT_SCH_EXIT_EID, CFE_EVS_CRITICAL, "KIT_SCH App: terminating, err = 0x%08X", Status);

    CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of KIT_SCH_Main() */


/******************************************************************************
** Function: KIT_SCH_NoOpCmd
**
*/

boolean KIT_SCH_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CFE_EVS_SendEvent (KIT_SCH_NOOP_INFO_EID,
                      CFE_EVS_INFORMATION,
                      "Kit Scheduler (KIT_SCH) version %d.%d received a no operation command",
                      KIT_SCH_MAJOR_VERSION,KIT_SCH_MINOR_VERSION);

   return TRUE;

} /* End KIT_SCH_NoOpCmd() */


/******************************************************************************
** Function: KIT_SCH_ResetAppCmd
**
*/

boolean KIT_SCH_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   CMDMGR_ResetStatus(CMDMGR_OBJ);
   TBLMGR_ResetStatus(TBLMGR_OBJ);

   MSGTBL_ResetStatus();
   SCHTBL_ResetStatus();

   SCHEDULER_ResetStatus();

   return TRUE;

} /* End KIT_SCH_ResetAppCmd() */


/******************************************************************************
** Function: KIT_SCH_SendHousekeepingPkt
**
*/
void KIT_SCH_SendHousekeepingPkt(void)
{

   /*
   ** KIT_SCH Data
   */

   KitSchHk.ValidCmdCnt   = KitSch.CmdMgr.ValidCmdCnt;
   KitSchHk.InvalidCmdCnt = KitSch.CmdMgr.InvalidCmdCnt;

   /*
   ** TBLMGR Data
   */

   KitSchHk.MsgTblLastLoadStatus = KitSch.MsgTbl.LastLoadStatus;
   KitSchHk.MsgTblAttrErrCnt     = KitSch.MsgTbl.AttrErrCnt;
   
   KitSchHk.SchTblLastLoadStatus = KitSch.SchTbl.LastLoadStatus;
   KitSchHk.SchTblAttrErrCnt     = KitSch.SchTbl.AttrErrCnt;

   /*
   ** Scheduler Data
   ** - At a minimum every scheduler variable effected by a reset must be included
   ** - These have been rearranged to align data words
   */

   KitSchHk.SlotsProcessedCount          = KitSch.Scheduler.SlotsProcessedCount;
   KitSchHk.ScheduleActivitySuccessCount = KitSch.Scheduler.ScheduleActivitySuccessCount;
   KitSchHk.ScheduleActivityFailureCount = KitSch.Scheduler.ScheduleActivityFailureCount;
   KitSchHk.ValidMajorFrameCount         = KitSch.Scheduler.ValidMajorFrameCount;
   KitSchHk.MissedMajorFrameCount        = KitSch.Scheduler.MissedMajorFrameCount;
   KitSchHk.UnexpectedMajorFrameCount    = KitSch.Scheduler.UnexpectedMajorFrameCount;
   KitSchHk.TablePassCount               = KitSch.Scheduler.TablePassCount;
   KitSchHk.ConsecutiveNoisyFrameCounter = KitSch.Scheduler.ConsecutiveNoisyFrameCounter;
   KitSchHk.SkippedSlotsCount            = KitSch.Scheduler.SkippedSlotsCount;
   KitSchHk.MultipleSlotsCount           = KitSch.Scheduler.MultipleSlotsCount;
   KitSchHk.SameSlotCount                = KitSch.Scheduler.SameSlotCount;
   KitSchHk.SyncAttemptsLeft             = KitSch.Scheduler.SyncAttemptsLeft;
   KitSchHk.LastSyncMETSlot              = KitSch.Scheduler.LastSyncMETSlot;
   KitSchHk.IgnoreMajorFrame             = KitSch.Scheduler.IgnoreMajorFrame;
   KitSchHk.UnexpectedMajorFrame         = KitSch.Scheduler.UnexpectedMajorFrame;

   CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &KitSchHk);
   CFE_SB_SendMsg((CFE_SB_Msg_t *) &KitSchHk);

} /* End KIT_SCH_SendHousekeepingPkt() */

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

    MSGTBL_Constructor(MSGTBL_OBJ, SCHEDULER_GetMsgTblPtr, SCHEDULER_LoadMsgTbl, SCHEDULER_LoadMsgTblEntry);
    SCHTBL_Constructor(SCHTBL_OBJ, SCHEDULER_GetSchTblPtr, SCHEDULER_LoadSchTbl, SCHEDULER_LoadSchTblEntry);

    SCHEDULER_Constructor(SCHEDULER_OBJ);

    /*
    ** Initialize application managers
    */

    CFE_SB_CreatePipe(&KitSch.CmdPipe, KIT_SCH_PIPE_DEPTH, KIT_SCH_PIPE_NAME);
    CFE_SB_Subscribe(KIT_SCH_CMD_MID, KitSch.CmdPipe);
    CFE_SB_Subscribe(KIT_SCH_SEND_HK_MID, KitSch.CmdPipe);

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, KIT_SCH_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, KIT_SCH_ResetAppCmd, 0);

    CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_SCH_LOAD_TBL_CMD_FC, TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, KIT_SCH_DUMP_TBL_CMD_FC, TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);

    CMDMGR_RegisterFunc(CMDMGR_OBJ, SCHEDULER_CFG_SCH_TBL_ENTRY_CMD_FC,  SCHEDULER_OBJ, SCHEDULER_ConfigSchEntryCmd, SCHEDULER_CFG_SCH_ENTRY_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, SCHEDULER_LOAD_SCH_TBL_ENTRY_CMD_FC, SCHEDULER_OBJ, SCHEDULER_LoadSchEntryCmd,   SCHEDULER_LOAD_SCH_ENTRY_CMD_DATA_LEN);

    CMDMGR_RegisterFunc(CMDMGR_OBJ, SCHEDULER_LOAD_MSG_TBL_ENTRY_CMD_FC, SCHEDULER_OBJ, SCHEDULER_LoadMsgEntryCmd, SCHEDULER_LOAD_MSG_ENTRY_CMD_DATA_LEN);
    
    CFE_SB_InitMsg(&KitSchHk, KIT_SCH_HK_TLM_MID, KIT_SCH_HK_TLM_LEN, TRUE);

    CFE_EVS_SendEvent(KIT_SCH_INIT_DEBUG_EID, KIT_SCH_INIT_EVS_TYPE,"KIT_SCH_InitApp() Before TBLMGR calls\n");
    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, MSGTBL_LoadCmd, MSGTBL_DumpCmd, KIT_SCH_DEF_MSG_TBL_FILE_NAME);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, SCHTBL_LoadCmd, SCHTBL_DumpCmd, KIT_SCH_DEF_SCH_TBL_FILE_NAME);

    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(KIT_SCH_INITSTATS_INF_EID,
                               CFE_EVS_INFORMATION,
                               "KIT_SCH Initialized. Version %d.%d.%d.%d",
                               KIT_SCH_MAJOR_VERSION,
                               KIT_SCH_MINOR_VERSION,
                               KIT_SCH_REVISION,
                               KIT_SCH_MISSION_REV);

    return(Status);

} /* End of InitApp() */


/******************************************************************************
** Function: ProcessCmdPkt
**
*/
static void ProcessCmdPkt(CFE_SB_MsgPtr_t CmdMsgPtr)
{

   CFE_SB_MsgId_t  MsgId;

   MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

   switch (MsgId)
   {
      case KIT_SCH_CMD_MID:
         CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
         break;

      case KIT_SCH_SEND_HK_MID:
         KIT_SCH_SendHousekeepingPkt();
         break;

      default:
         CFE_EVS_SendEvent(KIT_SCH_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                           "Received invalid command packet,MID = 0x%x",MsgId);

         break;

   } /* End Msgid switch */

   return;

} /* End ProcessCmdPkt() */


/* end of file */
