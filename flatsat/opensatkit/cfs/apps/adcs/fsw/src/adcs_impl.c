


#include "adcs_impl.h"
#include "adcs_io_lib.h"
#include "adcs_logger.h"
#include "FlatSatFsw.h"
/*
** Global File Data
*/

#if _FSS_CHALLENGE_ == 1
uint64_t adcs_flag = 0x0;
extern FSS_Cal ADCS_Fss_Calibration;
#endif // _FSS_CHALLENGE_

void FlatSatFswSetAdcsImpl(ADCS_IMPL_Class* ADCSImplInput);
static ADCS_IMPL_Class* ADCSImpl = NULL;

/******************************************************************************
** Function: ADCS_IMPL_Constructor
**
** Initialize a ADCS_IMPL object.
**
** Notes:
**   1. This must be called prior to any other function.
**   2. Assumes IF42 has constructed osk_42_lib's AC42 shared data structure 
**
*/
void ADCS_IMPL_Constructor(ADCS_IMPL_Class*  ADCSImplObj) {
    
    ADCSImpl = ADCSImplObj;

    /* If a class state variable can't default to zero then must be set after this */
    CFE_PSP_MemSet((void*)ADCSImpl, 0, sizeof(ADCS_IMPL_Class));
    
    /* Setup Configuration Table */
    ADCS_CONF_TBL_Constructor(&(ADCSImpl->ConfTbl), ADCS_IMPL_GetConfTblPtr, ADCS_IMPL_LoadConfTbl, ADCS_IMPL_LoadConfTblEntry);
    CFE_SB_InitMsg(&ADCSImpl->FswTlmPkt, ADCS_FSW_TLM_MID, ADCS_IMPL_TLM_FSW_PKT_LEN, TRUE);

    // Initialize ADCS HW Manager
    ADCS_HW_MGR_Constructor(&ADCSImpl->HW);
    #if defined(_FLATSAT_HARDWARE_FSW_)
    ADCS_HW_MGR_AddDevice(ADCS_HW_DEVICE_IMU_IDX, &ADCSImpl->IMU, &ADCSImpl->IMU.TlmPkt, 
        ADCS_HW_IMU_Init, ADCS_HW_IMU_Start, ADCS_HW_IMU_Stop, ADCS_HW_IMU_Reset, 
        ADCS_HW_IMU_ResetStatus, ADCS_HW_IMU_TLM_Update, ADCS_HW_IMU_Update, &ADCSImpl->ConfTbl.Data.Imu);
    CFE_SB_InitMsg(&ADCSImpl->IMU.TlmPkt, ADCS_HW_IMU_TLM_MID, ADCS_HW_IMU_TLM_SENSOR_PKT_LEN, TRUE);

    ADCS_HW_MGR_AddDevice(ADCS_HW_DEVICE_MTR_IDX, &ADCSImpl->MTR, &ADCSImpl->MTR.TlmPkt, 
        ADCS_HW_MTR_Init, ADCS_HW_MTR_Start, ADCS_HW_MTR_Stop, ADCS_HW_MTR_Reset, 
        ADCS_HW_MTR_ResetStatus, ADCS_HW_MTR_TLM_Update, ADCS_HW_MTR_Update, &ADCSImpl->ConfTbl.Data.Mtr);
    CFE_SB_InitMsg(&ADCSImpl->MTR.TlmPkt, ADCS_HW_MTR_TLM_MID, ADCS_HW_MTR_TLM_SENSOR_PKT_LEN, TRUE);

    ADCS_HW_MGR_AddDevice(ADCS_HW_DEVICE_RW_IDX, &ADCSImpl->RW, &ADCSImpl->RW.TlmPkt, 
        ADCS_HW_RW_Init, ADCS_HW_RW_Start, ADCS_HW_RW_Stop, ADCS_HW_RW_Reset, 
        ADCS_HW_RW_ResetStatus, ADCS_HW_RW_TLM_Update, ADCS_HW_RW_Update, &ADCSImpl->ConfTbl.Data.Wheel);
    CFE_SB_InitMsg(&ADCSImpl->RW.TlmPkt, ADCS_HW_RW_TLM_MID, ADCS_HW_RW_TLM_PKT_LEN, TRUE);

    ADCS_HW_MGR_AddDevice(ADCS_HW_DEVICE_XADC_IDX, &ADCSImpl->XADC, &ADCSImpl->XADC.TlmPkt, 
        ADCS_HW_XADC_Init, ADCS_HW_XADC_Start, ADCS_HW_XADC_Stop, ADCS_HW_XADC_Reset, 
        ADCS_HW_XADC_ResetStatus, ADCS_HW_XADC_TLM_Update, ADCS_HW_XADC_Update, &ADCSImpl->ConfTbl.Data.Analog);
    CFE_SB_InitMsg(&ADCSImpl->XADC.TlmPkt, ADCS_HW_XADC_TLM_MID, ADCS_HW_XADC_TLM_SENSOR_PKT_LEN, TRUE);

    ADCS_HW_MGR_AddDevice(ADCS_HW_DEVICE_ST_IDX, &ADCSImpl->ST, &ADCSImpl->ST.TlmPkt, 
        ADCS_HW_ST_Init, ADCS_HW_ST_Start, ADCS_HW_ST_Stop, ADCS_HW_ST_Reset, 
        ADCS_HW_ST_ResetStatus, ADCS_HW_ST_TLM_Update, ADCS_HW_ST_Update, &ADCSImpl->ConfTbl.Data.StarTracker);
    CFE_SB_InitMsg(&ADCSImpl->ST.TlmPkt, ADCS_HW_ST_TLM_MID, ADCS_HW_ST_TLM_PKT_LEN, TRUE);
    #endif

    #if defined(_FLATSAT_SIM_FSW_)
    /* XADCS FOR SIM FSW Includes Just Voltages and Temperatures */
    ADCS_HW_MGR_AddDevice(ADCS_HW_DEVICE_XADC_IDX, &ADCSImpl->XADC, &ADCSImpl->XADC.TlmPkt, 
        ADCS_HW_XADC_Init, ADCS_HW_XADC_Start, ADCS_HW_XADC_Stop, ADCS_HW_XADC_Reset, 
        ADCS_HW_XADC_ResetStatus, ADCS_HW_XADC_TLM_Update, ADCS_HW_XADC_Update, &ADCSImpl->ConfTbl.Data.Analog);
    CFE_SB_InitMsg(&ADCSImpl->XADC.TlmPkt, ADCS_HW_XADC_TLM_MID, ADCS_HW_XADC_TLM_SENSOR_PKT_LEN, TRUE);
    
    #if _FSS_CHALLENGE_ == 1
    ADCS_HW_MGR_AddDevice(ADCS_HW_DEVICE_FSS_IDX, &ADCSImpl->FSS, &ADCSImpl->FSS.TlmPkt, 
        ADCS_HW_FSS_Init, ADCS_HW_FSS_Start, ADCS_HW_FSS_Stop, ADCS_HW_FSS_Reset, 
        ADCS_HW_FSS_ResetStatus, ADCS_HW_FSS_TLM_Update, ADCS_HW_FSS_Update, &ADCSImpl->ConfTbl.Data.Fss);

    int32 FlagFileHandle;

    FlagFileHandle = OS_open("/cf/flag.txt", OS_READ_ONLY, 0);

    if (FlagFileHandle >= OS_FS_SUCCESS){
        int ret = OS_read(FlagFileHandle, &adcs_flag, sizeof(adcs_flag));
        if(ret != sizeof(adcs_flag)){
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_ERROR, "Flag file does not match size of flag...");
        }
        OS_close(FlagFileHandle);
    }
    else{
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_ERROR, "Could not load flag...");
    }

    CFE_SB_InitMsg(&ADCSImpl->FSS.TlmPkt, ADCS_HW_FSS_TLM_MID, ADCS_HW_FSS_TLM_SENSOR_PKT_LEN, TRUE);

    #endif // _FSS_CHALLENGE_

    #endif

    // Set Startup Control Mode
    ADCSImpl->CtrlMode = 0;
    ADCSImpl->Init = true;

    // Allocate Space for Dynamic FSW Structures
    FlatSatFswSetAdcsImpl(ADCSImpl);
    FlatSatFswAllocateAC(&ADCSImpl->AC);
    FlatSatFswInitAC(&ADCSImpl->AC);

} /* End ADCS_IMPL_Constructor() */

int ADCS_IMPL_ProcessSensorSb(void) {
    int32           CfeStatus;
    CFE_SB_Msg_t*   SbMsgPtr;
    ADCS_SensorDataPkt_t * SensorDataPkt;
    CFE_SB_MsgId_t  MsgId;
    uint16          MsgLen;
    // uint16          ExecuteCycle = 0;
    // boolean         ExecuteLoop  = FALSE;

    // CfeStatus = CFE_SB_RcvMsg(&SbMsgPtr, ADCSImpl->SensorPipe, 50);
    // Wait until we get updated sensor data before continueing forward
    CfeStatus = CFE_SB_RcvMsg(&SbMsgPtr, ADCSImpl->SensorPipe, CFE_SB_PEND_FOREVER);

    if (CfeStatus == CFE_SUCCESS ) {
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "**** ADCS_IMPL_ProcessSensorSb() Recieved SensorDataPkt SB Msg");
        
        MsgId = CFE_SB_GetMsgId(SbMsgPtr); 
        MsgLen = CFE_SB_GetUserDataLength(SbMsgPtr);
        ADCS_LOG_DEBUG("**** ADCS_IMPL_ProcessSensorSb() Recieved SensorDataPkt SB Msg with MsgId 0x%04X, MsgLen %d");
        if (MsgId == ADCS_SIM_SENSOR_DATA_MID) {
            SensorDataPkt = (ADCS_SensorDataPkt_t *) SbMsgPtr;
            if(MsgLen> 0) {
                StringToAcType(SensorDataPkt->Data, MsgLen, &ADCSImpl->AC);
            } else {
                CfeStatus = -1;
            }
        }
    } else {
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "**** ADCS_IMPL_ProcessSensorSb() Timeout on SensorDataPkt SB Msg");
    }
    return CfeStatus;
}

void ADCS_IMPL_Run(void) {
    // ADCS_LOG_INFO("Starting ADCS RunLoop");
    if(ADCSImpl->AC.FsCtrl.Debug) {
        ADCS_LOG_INFO("ADCS_IMPL_Run() Started");
    }
    int32 Status;
    CFE_TIME_SysTime_t time_val = CFE_TIME_GetTime();
    uint32_t usecs = CFE_TIME_Sub2MicroSecs(time_val.Subseconds);
    double time_secs = (double) time_val.Seconds + ((double)usecs / 1E6); // convert to seconds
    ADCSImpl->FswTlmPkt.CtrlCfsExeTime = time_secs;
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "**** ADCS_IMPL_Run() (ExeCnt: %d, Time: %f) ****", (int)ADCSImpl->CtrlExeCnt, time_secs);
    if (ADCSImpl->Init) {
        ADCS_IMPL_CompleteStartup();
    }
    #if ADCS_HW_DEVICE_CNT_MAX > 0
    Status = ADCS_HW_MGR_UpdateHardware();
    #endif
    #ifdef _FLATSAT_SIM_FSW_
    Status = ADCS_IMPL_ProcessSensorSb();
    if(Status == CFE_SUCCESS && ADCSImpl->AC.Init == 0) {
       ADCSImpl->TimeStart = ADCSImpl->AC.Time;
    } else {
        if(!CompareDouble((ADCSImpl->AC.Time + 1), ADCSImpl->TimeStart, 1E-9)) {
            ADCSImpl->TimeStart = ADCSImpl->AC.Time;
        }
    }
    #endif
    if (Status == CFE_SUCCESS) {
        Status = FlatSatFsw(&ADCSImpl->AC);
        Status = ADCS_HW_MGR_UpdateTlm();
        ADCSImpl->CtrlExeCnt++;
    } else {
        CFE_EVS_SendEvent(ADCS_IMPL_ERROR_EID, CFE_EVS_DEBUG, "**** ADCS_IMPL_Run() ADCS Sensor Data Update Failure");
    }
    if(ADCSImpl->AC.FsCtrl.Debug) {
        ADCS_LOG_INFO("ADCS_IMPL_Run() Completed.");
    }

    // ADCS_LOG_INFO("Finished ADCS RunLoop");
}

boolean ADCS_IMPL_CompleteStartup(void) {
    // int32 Status = CFE_SUCCESS; // TODO: Ask Logan about this  | Function should just be void
    if (ADCSImpl->CtrlExeCnt == 0) {
        if (ADCSImpl->ConfTbl.Data.HardwareStartCnt > 0) {
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_INFORMATION, "ADCSImpl Initialize Hardware on First Run. Device Cnt %d", ADCSImpl->ConfTbl.Data.HardwareStartCnt);
        
            int hwStatus = 0;
            for(int i=0; i<ADCSImpl->ConfTbl.Data.HardwareStartCnt; i++) {
                hwStatus += ADCS_HW_MGR_InitHardware(ADCSImpl->ConfTbl.Data.HardwareIdxToStart[i]);
                hwStatus += ADCS_HW_MGR_StartHardware(ADCSImpl->ConfTbl.Data.HardwareIdxToStart[i]);
            }
            if (hwStatus =! 0) {
                CFE_EVS_SendEvent(ADCS_IMPL_ERROR_EID, CFE_EVS_ERROR, "**** ADCS_IMPL Start Hardware on Runtime Error");
            }
        }
    }
    ADCSImpl->Init = false;

    return CFE_SUCCESS; // TODO: Logan
}

boolean ADCS_IMPL_HardwareCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    const ADCS_IMPL_HardwareCmdPkt* CmdParam = (const ADCS_IMPL_HardwareCmdPkt *) MsgPtr;
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
        "Recieved ADCS HW Command. Cmd: %d, Component: %d", CmdParam->HwCmd, CmdParam->Component);
    int32 Status = CFE_SUCCESS;
    // #ifdef _FLATSAT_HARDWARE_FSW_
    if(CmdParam->Component >= ADCS_HW_TLM_ENABLE) {
        Status = ADCS_IMPL_HardwareTlmEnable(CmdParam->HwCmd, CmdParam->Component - ADCS_HW_TLM_ENABLE);
        return Status;
    }

    switch (CmdParam->HwCmd)
    {
    case HW_STOP:
        Status = ADCS_HW_MGR_StopHardware(CmdParam->Component);
        break;
    case HW_START:
        Status = ADCS_HW_MGR_StartHardware(CmdParam->Component);
        break;
    case HW_INIT:
        Status = ADCS_HW_MGR_InitHardware(CmdParam->Component);
        break;
    case HW_RESET:
        Status = ADCS_HW_MGR_ResetHardware(CmdParam->Component);
        break;
    case HW_RESET_STATUS:
        Status = ADCS_HW_MGR_ResetStatus(CmdParam->Component);
        break;
    default:
        break;
    }

    if (Status != CFE_SUCCESS) {
        CFE_EVS_SendEvent(ADCS_IMPL_CMD_ERR_EID, CFE_EVS_ERROR, "ADCS HW CMD Error. Cmd %d, Component %d, Status=0x%8X", CmdParam->HwCmd, CmdParam->Component, (int)Status);    
    }
    // #endif

    return Status;

}

/******************************************************************************
** Function: ADCS_IMPL_HardwareTlmEnable
**
*/
boolean ADCS_IMPL_HardwareTlmEnable(bool enable, uint8_t component) {
    #ifdef _FLATSAT_HARDWARE_FSW_
    if(component >= 0 && component <= (ADCSImpl->HW.DeviceCnt - 1)) {
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
         "Recieved HW TLM Enable Command. Enable: %d, Component Idx: %d", enable, component);
        ADCSImpl->HW.Devices[component].TlmEnabled = enable;
    } else {
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_ERROR,
            "Recieved Invalid HW TLM Enable Command. Enable: %d, Component Idx: %d", enable, component);
    }
    #endif
    return CFE_SUCCESS;
}

/******************************************************************************
** Function: ADCS_IMPL_TorqueRodStateCmd
**
*/
boolean ADCS_IMPL_TorqueRodStateCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    int32 Status = CFE_SUCCESS;
    #ifdef _FLATSAT_HARDWARE_FSW_
    const ADCS_IMPL_TorqueRodStateCmdPkt* CmdParam = (const ADCS_IMPL_TorqueRodStateCmdPkt *) MsgPtr;
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
        "Recieved ADCS Torque Rod State Command. Id: %d, State: %d", CmdParam->Id, CmdParam->State);
    if ((CmdParam->Id >= 0) && (CmdParam->Id <= ADCSImpl->MTR.MtrCount)) {
        if (ADCSImpl->MTR.Mtr[CmdParam->Id].active) {
            ADCSImpl->MTR.MtrState[CmdParam->Id] = CmdParam->State;
            Status = mtr_set_state(&ADCSImpl->MTR.Mtr[CmdParam->Id], CmdParam->State);
        } else {
            Status = ADCS_HW_EXEC_FAILURE;
        }
    } 
    
    if (Status != ADCS_HW_EXEC_SUCCESS) {
        CFE_EVS_SendEvent(ADCS_IMPL_CMD_ERR_EID, CFE_EVS_ERROR, "ADCS MTR State Cmd Error. Status=0x%8X", (int)Status);    
    }
    #endif
    return Status;

}

/******************************************************************************
** Function: ADCS_IMPL_SetCtrlMode
**
*/
boolean ADCS_IMPL_SetCtrlMode(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    const ADCS_IMPL_SetCtrlModeCmdPkt* CmdParam = (const ADCS_IMPL_SetCtrlModeCmdPkt *) MsgPtr;
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
        "Recieved ADCS Ctrl Mode Command. New Mode: %d", CmdParam->NewMode);
    ADCSImpl->CtrlMode = CmdParam->NewMode;
    ADCSImpl->AC.FsCtrl.CtrlModeInit = TRUE;
    return CFE_SUCCESS;
}

/******************************************************************************
** Function: ADCS_IMPL_StarTrackerRegisterCmd
**
*/
boolean ADCS_IMPL_StarTrackerRegisterCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    boolean status = CFE_SUCCESS;
    #ifdef _FLATSAT_HARDWARE_FSW_
    const ADCS_IMPL_StarTrackerRegisterCmdPkt* CmdParam = (const ADCS_IMPL_StarTrackerRegisterCmdPkt *) MsgPtr;
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
        "Recieved ADCS Star Tracker Register Access Command: IsWrite: %d, Address: 0x%08X, Data: 0x%08X", CmdParam->IsWrite, CmdParam->Addr, CmdParam->Data);
    
    register_data_t reg = {0, 0, 0, 0};
    if(ADCSImpl->ST.Active && kdst_is_active(ADCSImpl->ST.Kdst)) {
        reg.address = CmdParam->Addr;
        reg.valid = false;
        if(CmdParam->IsWrite) {
            reg = kdst_write_register(ADCSImpl->ST.Kdst, CmdParam->Addr, CmdParam->Data);
        } else {
            reg = kdst_read_register(ADCSImpl->ST.Kdst, CmdParam->Addr);
        }
        if(reg.valid) {
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION, "ADCS Star Tracker Register Access Command Successful. IsWrite: %d, Valid: %d, Address: 0x%08X, Data: 0x%08X", 
                reg.is_write,
                reg.valid,
                reg.address,
                reg.data);
            status = CFE_SUCCESS;
        } else {
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_ERROR, "ADCS Star Tracker Register Access Command Failed");
        }
        memcpy(&ADCSImpl->ST.StData->register_access_result, &reg, sizeof(register_data_t));
    } else {
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_ERROR, "ADCS Star Tracker Register Access Command Failed. ST not active");
    }
    #endif
    return status;
}
/******************************************************************************
** Function: ADCS_IMPL_UpdateFlag
** // _ADCS_CHALLENGE_
*/
#if _FSS_CHALLENGE_ == 1
boolean ADCS_IMPL_UpdateFlagCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    int32 FlagFileHandle = 0;
    boolean status = CFE_SUCCESS;
    const ADCS_IMPL_UpdateFlagCmdPkt *updateFlagMsg = (const ADCS_IMPL_UpdateFlagCmdPkt*)MsgPtr;

    if (!updateFlagMsg->update){
        return status;
    }

    FlagFileHandle = OS_open("/cf/flag.txt", OS_READ_ONLY, 0);

    if (FlagFileHandle >= OS_FS_SUCCESS){
        int ret = OS_read(FlagFileHandle, &adcs_flag, sizeof(adcs_flag));
        if(ret != sizeof(adcs_flag)){
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_ERROR, "Flag file does not match size of flag...");
        }
        OS_close(FlagFileHandle);
    }
    else{
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_ERROR, "Could not load flag...");
    }

    OS_printf("New flag\n");
    return status;
}
#endif // _FSS_CHALLENGE_
/******************************************************************************
** Function: ADCS_IMPL_FineSunSensorRegisterCmd
** // _ADCS_CHALLENGE_
*/
boolean ADCS_IMPL_FSSCalibrationCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

    boolean status = CFE_SUCCESS;

    ADCS_FSS_Class* FSS = (ADCS_FSS_Class *)&ADCSImpl->FSS;
    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
        if(FSS->Active < HW_INITIALIZED){
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION, "FSS Hardware not initialized. Cannot calibrate. | Status: %d", FSS->Active);
            return;
        }

        ADCSImpl->FSS.Cal = &ADCS_Fss_Calibration;

        struct Cal{
            char cal_buffer[64];
            FSS_Cal *cal_ptr;
        } OS_PACK cal;

        cal = (struct Cal){0};
        cal.cal_ptr = ADCSImpl->FSS.Cal;

        const ADCS_IMPL_FineSunSensorCalibrationCmdPkt* CmdParam = (const ADCS_IMPL_FineSunSensorCalibrationCmdPkt *) MsgPtr;

        CFE_PSP_MemCpy(cal.cal_buffer, CmdParam->CalibrationMatrix, 64);

        // Info leak here. Keeps going until NULL byte
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
                "FSS Cal: Type: %d, Calibration Matrix: [%s]", 
                    cal.cal_ptr->CalibrationType, 
                    cal.cal_buffer);

        #ifdef _FLATSAT_SIM_FSW_
            switch(CmdParam->CalibrationType){
                case 0: // Base calibration
                    ADCSImpl->FSS.Cal->CalibrationType = CmdParam->CalibrationType;
                    
                    sscanf(cal.cal_buffer, "%f, %f, %f", &ADCSImpl->FSS.Cal->Calibration[0], &ADCSImpl->FSS.Cal->Calibration[1], &ADCSImpl->FSS.Cal->Calibration[2]);


                    break;
                case 1:; // Scalar calibration
                    ADCSImpl->FSS.Cal->CalibrationType = CmdParam->CalibrationType;

                    // #pragma GCC diagnostic push
                    // #pragma GCC diagnostic ignored "-Wformat" // Yes we know this code is wrong be quiet

                    // Scan and put into pointer
                    sscanf(cal.cal_buffer, "%f, %f, %f", *((uint32_t*)(&ADCSImpl->FSS.Cal->Calibration[0])), *((uint32_t*)(&ADCSImpl->FSS.Cal->Calibration[1])), *((uint32_t*)(&ADCSImpl->FSS.Cal->Calibration[2])));
                    // #pragma GCC diagnostic push

                    break;

                default:
                    ADCSImpl->FSS.Cal->CalibrationType = 0;
                    
                    break;
            }
        #endif  // !_FLATSAT_HARDWARE_FSW_
        
    #endif // _FSS_CHALLENGE_

    return status;
}

/******************************************************************************
** Function: ADCS_IMPL_SetCtrlTarget
**
*/
boolean ADCS_IMPL_SetCtrlTarget(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    const ADCS_IMPL_SetCtrlTgtCmdPkt* CmdParam = (const ADCS_IMPL_SetCtrlTgtCmdPkt *) MsgPtr;
    boolean RetStatus = FALSE;
    if (CmdParam->FrameTgt == FRAME_L) {
        // Rectify and report target quat
        double qtr[4];
        int i;
        for(i=0; i<4; i++) {
            qtr[i] = (double) CmdParam->Qtr[i];
        }
        UNITQ(qtr);
        RECTIFYQ(qtr);
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
            "Recieved ADCS Ctrl Tgt Command. Target Frame: %d, Target Quaternion [x, y, z, s]: [%f, %f, %f, %f]",
            CmdParam->FrameTgt, qtr[0], qtr[1], qtr[2], qtr[3]);
            ADCSImpl->AC.FsCtrl.FrameTgt = CmdParam->FrameTgt;
            for (int i=0; i<4; i++) {
                ADCSImpl->AC.FsCtrl.qtr[i] = qtr[i];
            }
            RetStatus = TRUE;
    } else {
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_ERROR,
            "Recieved ADCS Ctrl Tgt Command with unsupported target control frame: %d", CmdParam->FrameTgt);
    }
    return RetStatus;
}

/******************************************************************************
** Function: ADCS_IMPL_GetConfTblPtr
**
*/
boolean ADCS_IMPL_WheelSpeedCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

    int Status;
    const ADCS_IMPL_WheelSpeedCmdPkt* CmdParam = (const ADCS_IMPL_WheelSpeedCmdPkt*) MsgPtr;
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
        "Recieved ADCS Wheel Speed Command. WheelId: %d, WheelSpeed: %d rpm, Control Mode: %d", CmdParam->WheelId, CmdParam->WheelSpeed, ADCSImpl->CtrlMode);
    if (ADCSImpl->CtrlMode != 2) {
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_ERROR,
            "Cannot Execute ADCS Wheel Speed Command. Current Ctrl Mode: %d does not match needed mode for open loop wheel ctrl (%d)", ADCSImpl->CtrlMode, FSW_CTRL_OPEN_LOOP_WHEEL);
        return ADCS_HW_EXEC_FAILURE;
    }
    ADCSImpl->AC.Whl[CmdParam->WheelId].Wcmd = CmdParam->WheelSpeed * RPM2RPS;
    // cubewheel_telecommand_t* tc = cubewheel_tc_set_reference_speed(CmdParam->WheelSpeed);
    // if(cubewheel_send_tc(&ADCSImpl->RW.Wheel[CmdParam->WheelId], tc)) {
    //     Status = CFE_SUCCESS;
    //     CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
    //         "ADCS Wheel Speed Command Successful");
    // } else {
    //     Status = ADCS_HW_EXEC_FAILURE;
    //     CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
    //         "ADCS Wheel Speed Command Failed");
    // }
    return Status;
}

/******************************************************************************
** Function: ADCS_IMPL_WheelRawCmd
**
*/
boolean ADCS_IMPL_WheelRawCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {

    int Status = CFE_SUCCESS;
    #ifdef _FLATSAT_HARDWARE_FSW_
    const ADCS_IMPL_WheelRawCmdPkt* CmdParam = (const ADCS_IMPL_WheelRawCmdPkt*) MsgPtr;
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
        "Recieved ADCS Wheel Raw Command. WheelId: %d, FrameId: %d, Data: [0x%02X,0x%02X,0x%02X,0x%02X,0x%02X,0x%02X]", 
        CmdParam->WheelId, 
        CmdParam->frameId, 
        CmdParam->data[0],
        CmdParam->data[1],
        CmdParam->data[2],
        CmdParam->data[3],
        CmdParam->data[4],
        CmdParam->data[5]);

    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    tc->frameId = CmdParam->frameId;
    tc->dataLen = CmdParam->dataLen;
    if(tc->dataLen <= CUBEWHEEL_TC_MAX_LEN) {
        memcpy(&tc->data, &CmdParam->data, tc->dataLen);
    } else {        
        Status = CFE_STATUS_WRONG_MSG_LENGTH;
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_ERROR,
            "Received a raw wheel command of size %d which is larger than max %d", sizeof(CmdParam->data), CUBEWHEEL_TC_MAX_LEN);
        return Status;
    }

    if(cubewheel_send_tc(&ADCSImpl->RW.Wheel[CmdParam->WheelId], tc)) {
        Status = CFE_SUCCESS;
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
            "ADCS Raw Wheel Command Successful");
    } else {
        Status = ADCS_HW_EXEC_FAILURE;
        CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
            "ADCS Raw Wheel Command Failed");
    }
    #endif
    return Status;
}

/******************************************************************************
** Function: ADCS_IMPL_CalibrationCmd
**
*/
boolean ADCS_IMPL_CalibrationCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    boolean Status = CFE_SUCCESS;
    #ifdef _FLATSAT_HARDWARE_FSW_
    const ADCS_IMPL_CalibrationCmdPkt* CmdParam = (const ADCS_IMPL_CalibrationCmdPkt*) MsgPtr;
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
        "Recieved ADCS Calibration Comand. Type: %d, Subtype: %d", CmdParam->CalibrationType, CmdParam->CalibrationSubtype); 
    switch(CmdParam->CalibrationType) 
    {
        case ADCS_CAL_CSS:
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
                "Perform CSS Calibration on next control loop eval");
            ADCSImpl->AC.FsCtrl.CalibrateCss = true;
            Status = CFE_SUCCESS;
            break;
        default:
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_ERROR,
                "Recieved ADCS Unknown ADCS Calibration Type");
            Status = -1;
    }
    #endif
    return Status;
}

/******************************************************************************
** Function: ADCS_IMPL_SunYawCtrlCmd
**
*/
boolean ADCS_IMPL_SunYawCtrlCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    boolean Status = -1;
    const ADCS_IMPL_SetSunYawCtrlCmdPkt* CmdParam = (const ADCS_IMPL_SetSunYawCtrlCmdPkt*) MsgPtr;
    if (CmdParam->SunYawTgt >= 0.0 && CmdParam->SunYawTgt < TWOPI && CmdParam->SunYawRateDeadband < 10*R2D) {
        if (CmdParam->SunYawDeadband < 5*D2R) {
            CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_CMD_EID, CFE_EVS_INFORMATION,
                "Recieved ADCS SunYawCtrl Cmd. SunYawTgt: %f rad (%f deg), SunYawDeadband: %f rad (%f deg), SunYawRateDeadband: %f rad/s (%f deg/s)", 
                CmdParam->SunYawTgt, CmdParam->SunYawTgt*R2D, CmdParam->SunYawDeadband, CmdParam->SunYawDeadband*R2D,
                CmdParam->SunYawRateDeadband, CmdParam->SunYawRateDeadband*R2D);
            ADCSImpl->AC.FsCtrl.SunYawTgt = CmdParam->SunYawTgt;
            ADCSImpl->AC.FsCtrl.SunYawDeadband = CmdParam->SunYawDeadband;
            ADCSImpl->AC.FsCtrl.SunYawRateDeadband = CmdParam->SunYawRateDeadband;
            Status = CFE_SUCCESS;
        }
    } else {
        CFE_EVS_SendEvent(ADCS_IMPL_CMD_ERR_EID, CFE_EVS_INFORMATION,
            "Recieved Invalid ADCS SunYawCtrl Cmd. SunYawTgt: %f, SunYawDeadband: %f. SunYawRateDeadband: %f",
            CmdParam->SunYawTgt, CmdParam->SunYawDeadband, CmdParam->SunYawRateDeadband);
    }
    return Status;
}

/******************************************************************************
** Function: ADCS_IMPL_SetCtrlParamsCmd
**
*/
boolean ADCS_IMPL_SetCtrlParamsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    const ADCS_IMPL_SetCtrlParamCmdPkt* CmdParam = (const ADCS_IMPL_SetCtrlParamCmdPkt*) MsgPtr;
    if (CmdParam != NULL ) {
        CFE_EVS_SendEvent(ADCS_IMPL_CMD_ERR_EID, CFE_EVS_INFORMATION,
            "Recieved SetCtrlParamsCmd. Debug: %d, Vmax: %f, Amax: %f, Wc: %f, Ixx: %f, Iyy: %f, Izz: %f",
            CmdParam->Debug, CmdParam->Vmax, CmdParam->Amax, CmdParam->Wc, 
            CmdParam->Ixx, CmdParam->Iyy, CmdParam->Izz);
        ADCSImpl->AC.FsCtrl.Debug = CmdParam->Debug;
        ADCSImpl->AC.FsCtrl.vmax = CmdParam->Vmax;
        ADCSImpl->AC.FsCtrl.amax = CmdParam->Amax;
        ADCSImpl->AC.FsCtrl.wc = CmdParam->Wc;
        ADCSImpl->AC.MOI[0][0] = CmdParam->Ixx;
        ADCSImpl->AC.MOI[1][1] = CmdParam->Iyy;
        ADCSImpl->AC.MOI[2][2] = CmdParam->Izz;
    }
    return CFE_SUCCESS;
}

/******************************************************************************
** Function: ADCS_IMPL_GetConfTblPtr
**
*/
const ADCS_CONF_TBL_Struct* ADCS_IMPL_GetConfTblPtr()
{

    return &(ADCSImpl->ConfTbl.Data);

} /* End ADCS_IMPL_GetConfTblPtr() */


/******************************************************************************
** Function: ADCS_IMPL_LoadConfTbl
**
** Notes:
**   1. This is a simple data copy. Pass/fail criteria could be added.
**   2. TODO - See file header TODO
** 
*/
boolean ADCS_IMPL_LoadConfTbl(ADCS_CONF_TBL_Struct* NewTbl)
{

    CFE_PSP_MemCpy(&(ADCSImpl->ConfTbl.Data), NewTbl, sizeof(ADCS_CONF_TBL_Struct));
    
    // int i;
    // Copy Config Data
    // ADCSImpl->IMU.Device.
    // ADCS_IMU_Class *IMU = &ADCSImpl->IMU;
    // for (i = 0; i<ADCSImpl->ConfTbl.Data.Imu.sensorCount; i++) {
    //     ImuSensor_Struct* sensorData = &ADCSImpl->ConfTbl.Data.Imu.imuSensors[i];
    //     bno08x_init_sensor_data(&IMU->Data[sensorData->sensorIdx], sensorData->sensorId, sensorData->sensorDataSize, sensorData->updateRate);
    // }
    
    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "ADCS_IMPL_LoadConfTbl()\n");

    return TRUE;

} /* End ADCS_IMPL_LoadConfTbl() */


/******************************************************************************
** Function: ADCS_IMPL_LoadConfTblEntry
**
** Notes:
**   1. This is a simple data copy. Pass/fail criteria could be added.
**   2. TODO - See file header TODO
*/
boolean ADCS_IMPL_LoadConfTblEntry(uint16 ObjId, void* ObjData)
{

    boolean RetStatus = TRUE;
    
    switch (ObjId) {
        
        case ADCS_CONF_TBL_OBJ_KP:
            CFE_PSP_MemCpy(&(ADCSImpl->ConfTbl.Data.Controller.Kp), ObjData, sizeof(Kp_Struct));
            break;
         
        case ADCS_CONF_TBL_OBJ_KR:
            CFE_PSP_MemCpy(&(ADCSImpl->ConfTbl.Data.Controller.Kr), ObjData, sizeof(Kr_Struct));
            break;
         
        case ADCS_CONF_TBL_OBJ_KUNL:
            CFE_PSP_MemCpy(&(ADCSImpl->ConfTbl.Data.Controller.Kunl), ObjData, sizeof(float));
            break;
      
        case ADCS_CONF_TBL_OBJ_HCMD_LIM:
            CFE_PSP_MemCpy(&(ADCSImpl->ConfTbl.Data.Controller.HcmdLim), ObjData, sizeof(HcmdLim_Struct));
            break;

        default:
          RetStatus = FALSE;
    
    } /* End ObjId switch */
    
    // TblToAcStruct();

    CFE_EVS_SendEvent(ADCS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "ADCS_IMPL_LoadConfTbl() Kp.X = %.6f, Kr.X = %.6f\n", 
                            ADCSImpl->ConfTbl.Data.Controller.Kp.X,ADCSImpl->ConfTbl.Data.Controller.Kr.X);
  
    return RetStatus;

} /* End ADCS_IMPL_LoadConfTblEntry() */


