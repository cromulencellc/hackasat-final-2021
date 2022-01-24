/* 
** Purpose: Implement a Eps_mgr application.
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
#include "eps_mgr_app.h"
#include "eps_logger.h"


/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessCommands(void);

/*
** Global Data
*/

EPS_MGR_Class  Eps_mgr;
EPS_MGR_HkPkt  Eps_mgrHkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(Eps_mgr.CmdMgr))
#define  TBLMGR_OBJ (&(Eps_mgr.TblMgr))


/******************************************************************************
** Function: EPS_MGR_AppMain
**
*/
void EPS_MGR_AppMain(void)
{

    int32  Status    = CFE_SEVERITY_ERROR;
    uint32 RunStatus = CFE_ES_APP_ERROR;


    CFE_ES_PerfLogEntry(EPS_MGR_MAIN_PERF_ID);
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
    double time_taken, time_rem;
    // double update_period = (double) (Adcs.ExecuteCycleDelay); // update period in msec
    unsigned int sleep_msec;

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
        clock_gettime(CLOCK_MONOTONIC, &Eps_mgr.LoopStart);
        CFE_ES_PerfLogEntry(EPS_MGR_MAIN_PERF_ID);
        #ifndef EPS_USE_SCHEDULER
        EPS_LOG_DEBUG("EPS_IMPL_Run()");
        Status = EPS_IMPL_Run();
        EPS_LOG_DEBUG("EPS ProcessCommands()");
        ProcessCommands();
        if (Status == CFE_SUCCESS) {
            EPS_LOG_DEBUG("EPS_IMPL_Update()");
            EPS_IMPL_Update();
        }
        clock_gettime(CLOCK_MONOTONIC, &Eps_mgr.LoopEnd);
        time_taken = (Eps_mgr.LoopEnd.tv_sec - Eps_mgr.LoopStart.tv_sec) * 1e3; 
        time_taken = (time_taken + (Eps_mgr.LoopEnd.tv_nsec - Eps_mgr.LoopStart.tv_nsec) * 1e-6);
        time_rem = (double) Eps_mgr.ExecuteCycleDelay  - time_taken;
        sleep_msec = (unsigned int)time_rem;
        CFE_EVS_SendEvent(EPS_MGR_DEBUG_EID, CFE_EVS_DEBUG, "EPS Runloop Execution Time, %f msec", time_taken);
        EPS_LOG_DEBUG("EPS Runloop Execution Time, %f msec, sleeping for %lu msec", time_taken, sleep_msec);
        OS_TaskDelay(sleep_msec);
        #endif
        CFE_ES_PerfLogExit(EPS_MGR_MAIN_PERF_ID);


    } /* End CFE_ES_RunLoop */


    /* Write to system log in case events not working */

    CFE_ES_WriteToSysLog("EPS_MGR Terminating, RunLoop status = 0x%08X\n", RunStatus);

    CFE_EVS_SendEvent(EPS_MGR_EXIT_ERR_EID, CFE_EVS_CRITICAL, "EPS_MGR Terminating,  RunLoop status = 0x%08X", RunStatus);

    CFE_ES_PerfLogExit(EPS_MGR_MAIN_PERF_ID);
    CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of EPS_MGR_Main() */


/******************************************************************************
** Function: EPS_MGR_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean EPS_MGR_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

    CFE_EVS_SendEvent (EPS_MGR_CMD_NOOP_INFO_EID,
                        CFE_EVS_INFORMATION,
                        "No operation command received for EPS_MGR version %d.%d",
                        EPS_MGR_MAJOR_VERSION,EPS_MGR_MINOR_VERSION);

   return TRUE;


} /* End EPS_MGR_NoOpCmd() */


/******************************************************************************
** Function: EPS_MGR_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean EPS_MGR_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

    CMDMGR_ResetStatus(CMDMGR_OBJ);
    TBLMGR_ResetStatus(TBLMGR_OBJ);

    return TRUE;

} /* End EPS_MGR_ResetAppCmd() */


/******************************************************************************
** Function: EPS_MGR_SendHousekeepingPkt
**
*/
void EPS_MGR_SendHousekeepingPkt(void)
{

    /* Good design practice in case app expands to more than one table */
    const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);

    
    /*
    ** CMDMGR Data
    */

    Eps_mgrHkPkt.ValidCmdCnt   = Eps_mgr.CmdMgr.ValidCmdCnt;
    Eps_mgrHkPkt.InvalidCmdCnt = Eps_mgr.CmdMgr.InvalidCmdCnt;

    
    /*
    ** EXTBL/EXOBJ Data
    ** - At a minimum all OBJECT variables effected by a reset must be included
    */

    Eps_mgrHkPkt.LastAction       = LastTbl->LastAction;
    Eps_mgrHkPkt.LastActionStatus = LastTbl->LastActionStatus;
    Eps_mgrHkPkt.EpsImplExeCnt = Eps_mgr.EpsImpl.CtrlExeCnt;


    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &Eps_mgrHkPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &Eps_mgrHkPkt);

} /* End EPS_MGR_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
    int32 Status = CFE_SUCCESS;

    Eps_mgr.ExecuteCycleDelay = EPS_MGR_RUNLOOP_DELAY; // Runloop delay in msec
    /*
    ** Initialize cFE interfaces 
    */

    CFE_SB_CreatePipe(&Eps_mgr.CmdPipe, EPS_MGR_CMD_PIPE_DEPTH, EPS_MGR_CMD_PIPE_NAME);
    CFE_SB_Subscribe(EPS_MGR_CMD_MID, Eps_mgr.CmdPipe);
    CFE_SB_Subscribe(EPS_MGR_SEND_HK_MID, Eps_mgr.CmdPipe);


    /*
    ** Initialize EPS MGR Impl 
    */
    EPS_IMPL_Constructor(&Eps_mgr.EpsImpl);

    /*
    ** Initialize App Framework Components 
    */

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,                     NULL,       EPS_MGR_NoOpCmd,                0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC,                    NULL,       EPS_MGR_ResetAppCmd,            0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EPS_MGR_LOAD_TBL_CMD_FC,                TBLMGR_OBJ, TBLMGR_LoadTblCmd,              TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EPS_MGR_DUMP_TBL_CMD_FC,                TBLMGR_OBJ, TBLMGR_DumpTblCmd,              TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EPS_IMPL_SET_MODE_CMD_FC,               NULL,       EPS_IMPL_SetModeCmd,            EPS_IMPL_SET_MODE_CMD_PKT_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, EPS_IMPL_SET_COMPONENT_STATE_CMD_FC,    NULL,       EPS_IMPL_SetComponentStateCmd,  EPS_IMPL_SET_COMPONENT_STATE_CMD_PKT_LEN);

    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, EPS_CFG_LoadCmd, EPS_CFG_DumpCmd, EPS_MGR_CFG_TBL_LOAD_FILE);

                            
    CFE_SB_InitMsg(&Eps_mgrHkPkt, EPS_MGR_TLM_HK_MID, EPS_MGR_TLM_HK_LEN, TRUE);

                        
    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(EPS_MGR_INIT_INFO_EID,
                                CFE_EVS_INFORMATION,
                                "EPS_MGR Initialized. Version %d.%d.%d.%d",
                                EPS_MGR_MAJOR_VERSION,
                                EPS_MGR_MINOR_VERSION,
                                EPS_MGR_REVISION,
                                EPS_MGR_MISSION_REV);

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

    Status = CFE_SB_RcvMsg(&CmdMsgPtr, Eps_mgr.CmdPipe, CFE_SB_POLL);

    do {
        if (Status == CFE_SUCCESS) {
            MsgId = CFE_SB_GetMsgId(CmdMsgPtr);

            switch (MsgId)
            {
                case EPS_MGR_CMD_MID:
                    CMDMGR_DispatchFunc(CMDMGR_OBJ, CmdMsgPtr);
                    break;

                case EPS_MGR_SEND_HK_MID:
                    EPS_MGR_SendHousekeepingPkt();
                    break;

                case EPS_EXECUTE_MID:
                    #ifdef EPS_USE_SCHEDULER
                        EPS_IMPL_Run();
                    #endif

                default:
                    CFE_EVS_SendEvent(EPS_MGR_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                                    "Received invalid command packet,MID = 0x%4X",MsgId);

                    break;

            } /* End Msgid switch */
        }


        // Get next message
        Status = CFE_SB_RcvMsg(&CmdMsgPtr, Eps_mgr.CmdPipe, CFE_SB_POLL);

    } while (Status == CFE_SUCCESS); /* End if SB received a packet */

} /* End ProcessCommands() */


/* end of file */
