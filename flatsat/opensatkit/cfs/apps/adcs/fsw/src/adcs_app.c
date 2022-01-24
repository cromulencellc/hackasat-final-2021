/* 
** Purpose: Implement a Adcs application.
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
#include "adcs_app.h"
#include "adcs_impl.h"
#include "time.h"

/*
** Local Function Prototypes
*/

static int32 InitApp(void);
static void ProcessSbMsg(void);
static void AdcsExecute(void);
static void AppTermCallback(void);

/*
** Global Data
*/

ADCS_Class  Adcs;
ADCS_HkPkt  AdcsHkPkt;

/*
** Convenience Macros
*/

#define  CMDMGR_OBJ (&(Adcs.CmdMgr))
#define  TBLMGR_OBJ (&(Adcs.TblMgr))
#define  ADCS_CONF_TBL     (&(Adcs.AdcsConfTbl))
#define  ADCSImpl          (&(Adcs.AdcsImpl))

/******************************************************************************
** Function: ADCS_AppMain
**
*/
void ADCS_AppMain(void)
{

    int32  Status    = CFE_SEVERITY_ERROR;
    uint32 RunStatus = CFE_ES_APP_ERROR;


    CFE_ES_PerfLogEntry(ADCS_MAIN_PERF_ID);
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
    double time_taken, time_rem;
    // double update_period = (double) (Adcs.ExecuteCycleDelay); // update period in msec
    unsigned int sleep_msec;
    /*
    ** Main process loop
    */
    while (CFE_ES_RunLoop(&RunStatus))
    {
        /*
        ** This is just a an example loop. There are many ways to control the
        ** main loop execution flow.
        */
        clock_gettime(CLOCK_MONOTONIC, &Adcs.LoopStart);
        CFE_ES_PerfLogEntry(ADCS_MAIN_PERF_ID);
        ProcessSbMsg();
        #ifndef ADCS_USE_SCHEDULER
        AdcsExecute();
        clock_gettime(CLOCK_MONOTONIC, &Adcs.LoopEnd);
        time_taken = (Adcs.LoopEnd.tv_sec - Adcs.LoopStart.tv_sec) * 1e3; 
        time_taken = (time_taken + (Adcs.LoopEnd.tv_nsec - Adcs.LoopStart.tv_nsec) * 1e-6);
        time_rem = (double) Adcs.ExecuteCycleDelay  - time_taken;
        sleep_msec = (unsigned int)time_rem;
        CFE_EVS_SendEvent(ADCS_APP_DEBUG_EID, CFE_EVS_DEBUG, "ADCS Runloop Execution Time, %f msec", time_taken);
        ADCS_LOG_DEBUG("ADCS Runloop Execution Time, %f msec, sleeping for %lu msec", time_taken, sleep_msec);
        OS_TaskDelay(sleep_msec);
        #endif
        CFE_ES_PerfLogExit(ADCS_MAIN_PERF_ID);

    } /* End CFE_ES_RunLoop */


    /* Write to system log in case events not working */

    CFE_ES_WriteToSysLog("ADCS Terminating, RunLoop status = 0x%08X\n", RunStatus);

    CFE_EVS_SendEvent(ADCS_EXIT_ERR_EID, CFE_EVS_CRITICAL, "ADCS Terminating,  RunLoop status = 0x%08X", RunStatus);

    CFE_ES_PerfLogExit(ADCS_MAIN_PERF_ID);
    CFE_ES_ExitApp(RunStatus);  /* Let cFE kill the task (and any child tasks) */

} /* End of ADCS_Main() */


/******************************************************************************
** Function: ADCS_NoOpCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean ADCS_NoOpCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

    CFE_EVS_SendEvent (ADCS_CMD_NOOP_INFO_EID,
                             CFE_EVS_INFORMATION,
                             "No operation command received for ADCS version %d.%d",
                             ADCS_MAJOR_VERSION,ADCS_MINOR_VERSION);

    return TRUE;


} /* End ADCS_NoOpCmd() */

/******************************************************************************
** Function: ADCS_SetExecutionDelay
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean ADCS_SetExecutionDelay(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
    boolean Status = FALSE;
    ADCS_SetExecutionDelayCmdPkt *CmdPkt = (ADCS_SetExecutionDelayCmdPkt*) MsgPtr;
    CFE_EVS_SendEvent (ADCS_SET_EXE_DEBUG_EID,
                             CFE_EVS_INFORMATION,
                             "ADCS Set Execution Delay. Set Delay Time to %d msec", CmdPkt->ExecutionDelayMsec);
    if(CmdPkt->ExecutionDelayMsec >= 100 && CmdPkt->ExecutionDelayMsec <= 2000) {
        CFE_EVS_SendEvent (ADCS_SET_EXE_DEBUG_EID,
                           CFE_EVS_INFORMATION,
                           "ADCS Set Execution Delay. Set Delay Time to %d msec", CmdPkt->ExecutionDelayMsec);
        Adcs.ExecuteCycleDelay = CmdPkt->ExecutionDelayMsec;
        Status = TRUE;
    }
    return Status;

} /* End ADCS_SetExecutionDelay() */


/******************************************************************************
** Function: ADCS_ResetAppCmd
**
** Function signature must match CMDMGR_CmdFuncPtr typedef 
*/

boolean ADCS_ResetAppCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

    CMDMGR_ResetStatus(CMDMGR_OBJ);
    TBLMGR_ResetStatus(TBLMGR_OBJ);
    ADCS_CONF_TBL_ResetStatus();

    return TRUE;

} /* End ADCS_ResetAppCmd() */


/******************************************************************************
** Function: ADCS_SendHousekeepingPkt
**
*/
void ADCS_SendHousekeepingPkt(void)
{

    /* Good design practice in case app expands to more than one table */
    const TBLMGR_Tbl* LastTbl = TBLMGR_GetLastTblStatus(TBLMGR_OBJ);

    
    /*
    ** CMDMGR Data
    */

    AdcsHkPkt.ValidCmdCnt   = Adcs.CmdMgr.ValidCmdCnt;
    AdcsHkPkt.InvalidCmdCnt = Adcs.CmdMgr.InvalidCmdCnt;
    
    /*
    ** ADCS_CONF_TBL Data
    ** - At a minimum all OBJECT variables effected by a reset must be included
    */

    AdcsHkPkt.LastAction       = LastTbl->LastAction;
    AdcsHkPkt.LastActionStatus = LastTbl->LastActionStatus;

    // ADCS Control TLM
    AdcsHkPkt.CtrlExeCnt = Adcs.AdcsImpl.CtrlExeCnt;
    AdcsHkPkt.CtrlMode = Adcs.AdcsImpl.CtrlMode;
    AdcsHkPkt.CtrlExeDelay = Adcs.ExecuteCycleDelay;

    // HW MGR TLM
    AdcsHkPkt.HwMgrExeCnt = Adcs.AdcsImpl.HW.TaskExeCnt;

    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) &AdcsHkPkt);
    CFE_SB_SendMsg((CFE_SB_Msg_t *) &AdcsHkPkt);

} /* End ADCS_SendHousekeepingPkt() */


/******************************************************************************
** Function: InitApp
**
*/
static int32 InitApp(void)
{
    int32 Status = CFE_SUCCESS;
    CFE_PSP_MemSet((void*)&Adcs, 0, sizeof(ADCS_Class));

    Adcs.ExecuteCycleDelay = ADCS_APP_RUNLOOP_DELAY;

    ADCS_IMPL_Constructor(ADCSImpl);

    /*
    ** Initialize cFE interfaces 
    */
    CFE_SB_CreatePipe(&Adcs.SbPipe, ADCS_SB_PIPE_DEPTH, ADCS_SB_PIPE_NAME);
    CFE_SB_Subscribe(ADCS_CMD_MID, Adcs.SbPipe);
    CFE_SB_Subscribe(ADCS_SEND_HK_MID, Adcs.SbPipe);
    #ifdef _FLATSAT_SIM_FSW_
    CFE_SB_CreatePipe(&ADCSImpl->SensorPipe, ADCS_SENSOR_PIPE_DEPTH, ADCS_SENSOR_PIPE_NAME);
    CFE_SB_Subscribe(ADCS_SIM_SENSOR_DATA_MID, ADCSImpl->SensorPipe);
    #endif
    /*
    ** Initialize App Framework Components 
    */

    CMDMGR_Constructor(CMDMGR_OBJ);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_NOOP_CMD_FC,  NULL, ADCS_NoOpCmd,     0);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, CMDMGR_RESET_CMD_FC, NULL, ADCS_ResetAppCmd, 0);
    
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_SET_EXE_DELAY_CMD_FC,  NULL, ADCS_SetExecutionDelay, ADCS_SET_EXECUTION_DELAY_CMD_DATA_LEN);
    // CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_CONF_TBL_CMD_FC,  TBLMGR_OBJ, TBLMGR_LoadTblCmd, TBLMGR_LOAD_TBL_CMD_DATA_LEN);
    // CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_CONF_TBL_DUMP_CMD_FC,  TBLMGR_OBJ, TBLMGR_DumpTblCmd, TBLMGR_DUMP_TBL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_HW_CMD_FC,  ADCSImpl, ADCS_IMPL_HardwareCmd, ADCS_IMPL_HARDWARE_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_CTRL_MODE_FC,  ADCSImpl, ADCS_IMPL_SetCtrlMode, ADCS_IMPL_SET_CTRL_MODE_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_CTRL_TGT_FC,  ADCSImpl, ADCS_IMPL_SetCtrlTarget, ADCS_IMPL_SET_CTRL_TGT_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_WHEEL_CMD_FC,  ADCSImpl, ADCS_IMPL_WheelSpeedCmd, ADCS_IMPL_WHEEL_SPEED_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_CTRL_PARAMS_CMD_FC,  ADCSImpl, ADCS_IMPL_SetCtrlParamsCmd, ADCS_IMPL_SET_CTRL_PARAM_CMD_DATA_LEN);

    #ifdef __FLATSAT_HARDWARE_FSW_
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_MTR_STATE_CMD_FC,  ADCSImpl, ADCS_IMPL_TorqueRodStateCmd, ADCS_IMPL_TORQUE_ROD_STATE_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_CAL_CMD_FC,  ADCSImpl, ADCS_IMPL_CalibrationCmd, ADCS_IMPL_CALIBRATION_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_SUN_YAW_CMD_FC,  ADCSImpl, ADCS_IMPL_SunYawCtrlCmd, ADCS_IMPL_SET_SUN_YAW_CTRL_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_WHEEL_CMD_RAW_FC,  ADCSImpl, ADCS_IMPL_WheelRawCmd, ADCS_IMPL_WHEEL_RAW_CMD_DATA_LEN);
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_ST_REG_CMD_FC,  ADCSImpl, ADCS_IMPL_StarTrackerRegisterCmd, ADCS_IMPL_STAR_TRACKER_REG_CMD_DATA_LEN);
    #endif
    // _ADCS_CHALLENGE_
    CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_FSS_CAL_CMD_FC,  ADCSImpl, ADCS_IMPL_FSSCalibrationCmd, ADCS_IMPL_FINE_SUN_SENSOR_CALIBRATION_CMD_DATA_LEN);

    #if _FSS_CHALLENGE_ == 1
        CMDMGR_RegisterFunc(CMDMGR_OBJ, ADCS_UPDATE_FLAG_CMD_FC,  ADCSImpl, ADCS_IMPL_UpdateFlagCmd, ADCS_IMPL_UPDATE_FLAG_CMD_DATA_LEN);
    #endif //_FSS_CHALLENGE_

    TBLMGR_Constructor(TBLMGR_OBJ);
    TBLMGR_RegisterTblWithDef(TBLMGR_OBJ, ADCS_CONF_TBL_LoadCmd, ADCS_CONF_TBL_DumpCmd, ADCS_CONF_TBL_DEF_LOAD_FILE);
                                
    CFE_SB_InitMsg(&AdcsHkPkt, ADCS_TLM_HK_MID, ADCS_TLM_HK_LEN, TRUE);

    OS_TaskInstallDeleteHandler((void *)(&AppTermCallback)); /* Call when application terminates */

    /*
    ** Application startup event message
    */
    Status = CFE_EVS_SendEvent(ADCS_INIT_INFO_EID,
                                        CFE_EVS_INFORMATION,
                                        "ADCS Initialized. Version %d.%d.%d.%d",
                                        ADCS_MAJOR_VERSION,
                                        ADCS_MINOR_VERSION,
                                        ADCS_REVISION,
                                        ADCS_MISSION_REV);

    return(Status);

} /* End of InitApp() */


static void AdcsExecute(void) {

    ADCS_IMPL_Run();

}

/******************************************************************************
** Function: ProcessSbMsg
**
*/
static void ProcessSbMsg(void)
{

    int32           CfeStatus;
    CFE_SB_Msg_t*   SbMsgPtr;
    CFE_SB_MsgId_t  MsgId;
    // uint16          ExecuteCycle = 0;
    // boolean         ExecuteLoop  = FALSE;

    CfeStatus = CFE_SB_RcvMsg(&SbMsgPtr, Adcs.SbPipe, CFE_SB_POLL);

    do {
        if (CfeStatus == CFE_SUCCESS ) {
            MsgId = CFE_SB_GetMsgId(SbMsgPtr); 
            // CFE_EVS_SendEvent(ADCS_APP_DEBUG_EID, CFE_EVS_DEBUG,
                            // "***ProcessSbPipe(): ExecuteLoop=%d,  ExecuteCycle=%d, MsgId=0x%04X",
                            // ExecuteLoop,ExecuteCycle,MsgId);
            
            switch (MsgId)
            {
                case ADCS_CMD_MID:
                    CMDMGR_DispatchFunc(CMDMGR_OBJ, SbMsgPtr);
                    break;

                case ADCS_SEND_HK_MID:
                    ADCS_SendHousekeepingPkt();
                    break;
                
                case ADCS_EXECUTE_MID:
                    #ifdef ADCS_USE_SCHEDULER
                        AdcsExecute();
                    #endif
                    break;

                default:
                    CFE_EVS_SendEvent(ADCS_CMD_INVALID_MID_ERR_EID, CFE_EVS_ERROR,
                                            "Received invalid command packet,MID = 0x%4X",MsgId);

                    break;
            } /* End Msgid switch */
        }
        // if (ExecuteLoop) {
        //     if (ExecuteCycle < Adcs.ExecuteMsgCycles) {
        //         OS_TaskDelay(Adcs.ExecuteCycleDelay);
        //         ADCS_IMPL_Run();
        //         ++ExecuteCycle;
        //     }
        //     else {
        //         ExecuteLoop = FALSE;
        //     }
        // }
        
        CfeStatus = CFE_SB_RcvMsg(&SbMsgPtr, Adcs.SbPipe, CFE_SB_POLL);
    
    // } while ( ExecuteLoop || (CfeStatus == CFE_SUCCESS) );
    } while ( CfeStatus == CFE_SUCCESS );
    
} /* End ProcessCommands() */


/******************************************************************************
** Function: AppTermCallback
**
** This function is called when the app is terminated. This shouldstatic void AppTermCallback(void);
*/
static void AppTermCallback(void) { 
    ADCS_IMPL_HardwareCmdPkt HwCmd;
    HwCmd.Component = ADCS_HW_ALL;
    HwCmd.HwCmd = HW_STOP;
    ADCS_IMPL_HardwareCmd(NULL, (CFE_SB_MsgPtr_t)&HwCmd);
} /* End AppTermCallback() */

/* end of file */
