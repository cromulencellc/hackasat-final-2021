#include "adcs_hw.h"
#include "cfe.h"

// Magnetic Torque Rod HW API
int32_t ADCS_HW_MTR_Init(void);
int32_t ADCS_HW_MTR_Start(void);
int32_t ADCS_HW_MTR_Reset(void);
int32_t ADCS_HW_MTR_Update(void);

/*
** Global File Data
*/
static ADCS_HW_MGR_Class* ADCS_HW = NULL;

// Local Function Prototypes
int32_t InitHardware(ADCS_HW_Device_t* Device);
int32_t StartHardware(ADCS_HW_Device_t* Device);
int32_t StopHardware(ADCS_HW_Device_t* Device);
int32_t ResetHardware(ADCS_HW_Device_t* Device);
int32_t ResetStatus(ADCS_HW_Device_t* Device);
int32_t RunHardware(ADCS_HW_Device_t* Device);
int32_t UpdateHardwareTlm(ADCS_HW_Device_t* Device);

void ADCS_HW_MGR_Constructor(ADCS_HW_MGR_Class*  ADCSHwMgrObj) {
    ADCS_HW = ADCSHwMgrObj;
    CFE_PSP_MemSet(ADCS_HW, 0, sizeof(ADCS_HW_MGR_Class));
    ADCS_HW->TaskExeCnt = 0;
}

void ADCS_HW_MGR_AddDevice(unsigned int deviceMgrIndex, ADCS_HW_DeviceClassPtr deviceClass, ADCS_HW_DeviceTlmClassPtr deviceTlmClass,
    ADCS_HW_Device_FuncPtr initFuncPtr, ADCS_HW_Device_FuncPtr startFuncPtr, ADCS_HW_Device_FuncPtr stopFuncPtr,
    ADCS_HW_Device_FuncPtr resetFuncPtr, ADCS_HW_Device_FuncPtr resetStatusFuncPtr, ADCS_HW_Device_FuncPtr tlmUpdateFuncPtr,
    ADCS_HW_Device_FuncPtr updateFuncPtr, ADCS_HW_DeviceConfTblPtr confTblPtr)
{
    int newDeviceCnt =  ADCS_HW->DeviceCnt + 1;
    if (newDeviceCnt > ADCS_HW_DEVICE_CNT_MAX) {
        CFE_EVS_SendEvent(ADCS_HW_ERROR_EID, CFE_EVS_ERROR,
            "Cannot add device, exceding max device count %d", ADCS_HW_DEVICE_CNT_MAX);
        return;
    }
    ADCS_HW->DeviceCnt = newDeviceCnt;
    
    CFE_PSP_MemSet(&ADCS_HW->Devices[deviceMgrIndex], 0, sizeof(ADCS_HW_Device_t));
    ADCS_HW->Devices[deviceMgrIndex].DeviceClassPtr = deviceClass;
    ADCS_HW->Devices[deviceMgrIndex].DeviceTlmPktPtr = deviceTlmClass;
    ADCS_HW->Devices[deviceMgrIndex].init = initFuncPtr;
    ADCS_HW->Devices[deviceMgrIndex].start = startFuncPtr;
    ADCS_HW->Devices[deviceMgrIndex].stop = stopFuncPtr;
    ADCS_HW->Devices[deviceMgrIndex].reset = resetFuncPtr;
    ADCS_HW->Devices[deviceMgrIndex].reset_status = resetStatusFuncPtr;
    ADCS_HW->Devices[deviceMgrIndex].tlm_update = tlmUpdateFuncPtr;
    ADCS_HW->Devices[deviceMgrIndex].update = updateFuncPtr;
    ADCS_HW->Devices[deviceMgrIndex].ConfTblPtr = confTblPtr;

    if(deviceMgrIndex >= 0 && deviceMgrIndex <= ( ADCS_HW->DeviceCnt - 1)) {
        ADCS_HW->Devices[deviceMgrIndex].TlmEnabled = true;
    }
}

int32_t ADCS_HW_MGR_InitHardware(uint8_t component) {
    int32_t Status;
    CFE_EVS_SendEvent(ADCS_HW_DEBUG_EID, CFE_EVS_INFORMATION,
        "Init HW Component -> %d", component);
     if(component == ADCS_HW_ALL) {
        for(int i=0; i<ADCS_HW->DeviceCnt; i++) {
            Status = InitHardware(&ADCS_HW->Devices[i]);
        }
    } else {
        if(component < (ADCS_HW->DeviceCnt)) {
             Status = InitHardware(&ADCS_HW->Devices[component]);
        } else {
            CFE_EVS_SendEvent(ADCS_HW_ERROR_EID, CFE_EVS_ERROR,
                "Unknown HW Component Init Sent %d", component);
        }
    }
    return Status;
}


int32_t ADCS_HW_MGR_StartHardware(uint8_t component) {
    int32_t Status;
    CFE_EVS_SendEvent(ADCS_HW_DEBUG_EID, CFE_EVS_INFORMATION,
        "Start HW Component -> %d", component);
    if(component == ADCS_HW_ALL) {
        for(int i=0; i<ADCS_HW->DeviceCnt; i++) {
            Status = StartHardware(&ADCS_HW->Devices[i]);
            OS_TaskDelay(ADCS_HW_LOOP_DELAY);
        }
    } else {
        if(component < (ADCS_HW->DeviceCnt)) {
             Status = StartHardware(&ADCS_HW->Devices[component]);
        } else {
            CFE_EVS_SendEvent(ADCS_HW_ERROR_EID, CFE_EVS_ERROR,
                "Unknown HW Component Start Sent %d", component);
        }
    }
    return Status;
}

int32_t ADCS_HW_MGR_StopHardware(uint8_t component) {
    int32_t Status;
    CFE_EVS_SendEvent(ADCS_HW_DEBUG_EID, CFE_EVS_INFORMATION,
        "Stop HW Component -> %d", component);
    if(component == ADCS_HW_ALL) {
        for(int i=0; i<ADCS_HW->DeviceCnt; i++) {
            Status = StopHardware(&ADCS_HW->Devices[i]);
        }
    } else {
        if(component < (ADCS_HW->DeviceCnt)) {
             Status = StopHardware(&ADCS_HW->Devices[component]);
        } else {
            CFE_EVS_SendEvent(ADCS_HW_ERROR_EID, CFE_EVS_ERROR,
                "Unknown HW Component Stop Sent %d", component);
        }
    }
    return Status;
}


int32_t ADCS_HW_MGR_ResetHardware(uint8_t component) {
    int32_t Status;
    CFE_EVS_SendEvent(ADCS_HW_DEBUG_EID, CFE_EVS_INFORMATION,
        "Reset HW Component -> %d", component);
    if(component == ADCS_HW_ALL) {
        for(int i=0; i<ADCS_HW->DeviceCnt; i++) {
            Status = ResetHardware(&ADCS_HW->Devices[i]);
        }
    } else {
        if(component < (ADCS_HW->DeviceCnt)) {
             Status = ResetHardware(&ADCS_HW->Devices[component]);
        } else {
            CFE_EVS_SendEvent(ADCS_HW_ERROR_EID, CFE_EVS_ERROR,
                "Unknown HW Component Reset Sent %d", component);
        }
    }
    return Status;
}


int32_t ADCS_HW_MGR_ResetStatus(uint8_t component) {
    int32_t Status;
    ADCS_HW->TaskExeCnt = 0;
    CFE_EVS_SendEvent(ADCS_HW_DEBUG_EID, CFE_EVS_INFORMATION,
        "ResetStatus HW Component -> %d", component);
    if(component == ADCS_HW_ALL) {
        for(int i=0; i<ADCS_HW->DeviceCnt; i++) {
            Status = ResetStatus(&ADCS_HW->Devices[i]);
        }
    } else {
        if(component < (ADCS_HW->DeviceCnt)) {
             Status = ResetStatus(&ADCS_HW->Devices[component]);
        } else {
            CFE_EVS_SendEvent(ADCS_HW_ERROR_EID, CFE_EVS_ERROR,
                "Unknown HW Component ResetStatus Sent %d", component);
        }
    }
    return Status;
}


int32_t ADCS_HW_MGR_UpdateHardware(void) {
    int32_t Status;
    CFE_EVS_SendEvent(ADCS_HW_DEBUG_EID, CFE_EVS_DEBUG,
                    "**** ADCS_HW_MGR_UpdateHardware(): ExecutionCnt: %d", ADCS_HW->TaskExeCnt);
    // ADCS_LOG_INFO("ADCS_HW_MGR_UpdateHardware() Started");
    for(int i=0; i<ADCS_HW->DeviceCnt; i++) {
        Status = RunHardware(&ADCS_HW->Devices[i]);
    }
    ADCS_HW->TaskExeCnt++;
    // ADCS_LOG_INFO("ADCS_HW_MGR_UpdateHardware() Completed");
    return Status;
}

int32_t ADCS_HW_MGR_UpdateTlm(void) {
    int32 Status = ADCS_HW_EXEC_SUCCESS;
    CFE_EVS_SendEvent(ADCS_HW_DEBUG_EID, CFE_EVS_DEBUG,
        "**** ADCS_HW_MGR_UpdateTlm()");
    for(int i=0; i<ADCS_HW->DeviceCnt; i++) {
        if(ADCS_HW->Devices[i].TlmEnabled) {
            if(ADCS_HW->Devices[i].Status >= HW_INITIALIZED) {
                Status = UpdateHardwareTlm(&ADCS_HW->Devices[i]);
                if (Status != ADCS_HW_EXEC_SUCCESS) {
                    CFE_EVS_SendEvent(ADCS_HW_ERROR_EID, CFE_EVS_ERROR,
                        "**** UpdateHardwareTlm() Error for Device Idx %d", i);
                }
            }
        }
    }
    return Status;
}

// Local Functions

int32_t InitHardware(ADCS_HW_Device_t* Device) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    if (!Device->Initialized) {
        Status = Device->init(Device);
    }
    if (Status == ADCS_HW_EXEC_SUCCESS) {
        Device->Status = HW_INITIALIZED;
        Device->Initialized = true;
    }
    return Status;
}

int32_t StartHardware(ADCS_HW_Device_t* Device) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    if (Device->Initialized && ( Device->Status == HW_STOPPED || Device->Status == HW_INITIALIZED || Device->Status != HW_ERROR )) {
        Status = Device->start(Device);
    } 
    if (Status == ADCS_HW_EXEC_SUCCESS) {
        Device->Status = HW_RUNNING;
    }
    return Status;
}

int32_t StopHardware(ADCS_HW_Device_t* Device) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    if (Device->Initialized && ( Device->Status == HW_RUNNING || Device->Status == HW_ERROR )) {
        Status = Device->stop(Device);
    } 
    if (Status == ADCS_HW_EXEC_SUCCESS) {
        Device->Status = HW_STOPPED;
    }
    return Status;
}

int32_t ResetHardware(ADCS_HW_Device_t* Device) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    // Device RUNNING, STOPPED, or ERROR
    if (Device->Initialized && ( Device->Status >= HW_RUNNING )) {
        Status = Device->stop(Device);
    }
    Device->DeviceResetCnt++;
    Status = Device->reset(Device);
    if (Status == ADCS_HW_EXEC_SUCCESS) {
        Device->Status =  HW_INITIALIZED;
    }
    return Status;
}

int32_t ResetStatus(ADCS_HW_Device_t* Device) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    Status = Device->reset_status(Device);
    Device->DeviceResetCnt = 0;
    Device->DeviceUpdateCnt = 0;
    return Status;
}

int32_t RunHardware(ADCS_HW_Device_t* Device) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    if (Device->Initialized && Device->Status == HW_RUNNING) {
        Status = Device->update(Device);
    }
    if (Status == ADCS_HW_EXEC_SUCCESS) {
        Device->DeviceUpdateCnt++;
    }
    return Status;
}

int32_t UpdateHardwareTlm(ADCS_HW_Device_t* Device) {
    int32_t Status = Device->tlm_update(Device);
    Device->TlmPktCnt++;
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) Device->DeviceTlmPktPtr);
    Status = CFE_SB_SendMsg((CFE_SB_Msg_t *) Device->DeviceTlmPktPtr);
    return Status;
}

int32_t ADCS_HW_MGR_UpdateCommonTlm(ADCS_HW_DeviceTlmCommon_t* tlm, ADCS_HW_Device_t* Device) {
    tlm->DeviceResetCnt = Device->DeviceResetCnt;
    tlm->DeviceUpdateCnt = Device->DeviceUpdateCnt;
    tlm->Initialized = (uint8_t) Device->Initialized;
    tlm->Status = (uint8_t) Device->Status;
    return ADCS_HW_EXEC_SUCCESS;
}

