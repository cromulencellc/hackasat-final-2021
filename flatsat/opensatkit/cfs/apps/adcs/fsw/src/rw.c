#include "rw.h"

/*
** Global File Data
*/
#define IDENTIFICATION_FRAME_COUNT 2
ADCS_RW_Class *RWDevice = NULL;
// static uint8_t identificationTlmFrameIds[IDENTIFICATION_FRAME_COUNT] = {TLM_WHEEL_IDENTIFICATION, TLM_WHEEL_EXTENDED_IDENTIFICATION}; 
/*
** Local Function Prototypes
*/

/*
** RW Hal Functions
*/
int32_t ADCS_HW_RW_Init(ADCS_HW_Device_t *self) {
    int32 Status = ADCS_HW_EXEC_SUCCESS;
    ADCS_RW_Class* RW = (ADCS_RW_Class *) self->DeviceClassPtr;
    RWDevice = RW; // Set file pointer for child task
    WheelData_Struct* rwConfTbl = (WheelData_Struct*) self->ConfTblPtr;
    RW->ControlMode = rwConfTbl->controlMode;
    RW->WheelCnt = rwConfTbl->wheelCount;
    // RW->PollRateMs = rwConfTbl->pollRateMs;
    bool wheelStatus = true;
    CFE_EVS_SendEvent(ADCS_HW_RW_ERROR_EID, CFE_EVS_INFORMATION,
        "ADCS_HW_RW_Init(). Wheel Cnd %d", RW->WheelCnt);

    if (Status == CFE_SUCCESS) {
        for (int i = 0; i<RW->WheelCnt; i++) {
                cubewheel_constructor(&RW->Wheel[i], i);
                cubewheel_override_sleep_fuction(&RW->Wheel[i], &OS_TaskDelay);
                CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG,
                    "RW %d Init. Status %d", i, wheelStatus);
                if(Status != OS_SUCCESS) {
                    wheelStatus &= false;
                    CFE_EVS_SendEvent(ADCS_HW_RW_ERROR_EID, CFE_EVS_ERROR,
                        "RW %d Init Failure. Status %d", i, wheelStatus);
                }
        }
    } else {
                CFE_EVS_SendEvent(ADCS_HW_RW_ERROR_EID, CFE_EVS_ERROR,
            "Failed to create %s semaphore. Status=0x%8X", ADCS_RW_CHILD_SEM_NAME, (int)Status);
        wheelStatus = false;
    }
    
    if(!wheelStatus) {
        CFE_EVS_SendEvent(ADCS_HW_RW_ERROR_EID, CFE_EVS_ERROR,
            "RW Init Failure");
        Status = ADCS_HW_EXEC_FAILURE;
    } else {
        Status = ADCS_HW_EXEC_SUCCESS;
    }
    
    return Status;
}

int32_t ADCS_HW_RW_Start(ADCS_HW_Device_t *self) {

    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    
    ADCS_RW_Class* RW = (ADCS_RW_Class *) self->DeviceClassPtr;
    WheelData_Struct* rwConfTbl = (WheelData_Struct*) self->ConfTblPtr;
    bool tcStatus;
    bool wheelStatus = true;
    int i;
    for (i=0; i<RW->WheelCnt; i++) {
        ADCS_LOG_INFO("Cubewheel Init /w I2C Addr: 0x%02X, GPIO Chip: %s, GPIO Line: %d", rwConfTbl->wheelData[i].i2cConfig.i2cAddr, rwConfTbl->wheelData[i].enableGpio.gpiochip, rwConfTbl->wheelData[i].enableGpio.linenum);
        cubewheel_init(&RW->Wheel[i], rwConfTbl->wheelData[i].i2cConfig.i2cPath, 
                    rwConfTbl->wheelData[i].i2cConfig.i2cAddr,
                    rwConfTbl->wheelData[i].i2cConfig.i2cType,
                    rwConfTbl->wheelData[i].enableGpio.gpiochip,
                    rwConfTbl->wheelData[i].enableGpio.linenum);
        wheelStatus  &= (RW->Wheel[i].gpioEnable.active && RW->Wheel[i].i2c.active && RW->Wheel[i].data.active);
        CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Active: %d",RW->Wheel[i].data.wheelId, RW->Wheel[i].data.active);
    }
    if(wheelStatus) {
        for (i=0; i<RW->WheelCnt; i++) {
            tcStatus = cubewheel_send_tc(&RW->Wheel[i], cubewheel_tc_set_control_mode( RW->ControlMode));
            if(RW->ControlMode == WHEEL_MODE_USE_SPEED_CONTROL && tcStatus) {
                tcStatus = cubewheel_send_tc(&RW->Wheel[i], cubewheel_tc_set_reference_speed((float) 0.0));
                CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Init Speed Control (0 rpm). TC Status %d",RW->Wheel[i].data.wheelId, tcStatus);
            } else if (RW->ControlMode == WHEEL_MODE_DUTY_CYCLE_CONTROL && tcStatus) 
            {
                tcStatus = cubewheel_send_tc(&RW->Wheel[i], cubewheel_tc_set_duty_cycle(0));
                CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Init Duty-Cycle Control (0%%). TC Staus %d",RW->Wheel[i].data.wheelId, tcStatus);
            } else if ((RW->ControlMode == WHEEL_MODE_IDLE || RW->ControlMode == WHEEL_MODE_NO_CONTROL) && tcStatus)
            {
                CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Init IDLE or NO-CONTROL. TC Staus %d",RW->Wheel[i].data.wheelId, tcStatus);
            } 
            if (tcStatus) {
                RW->Active = true;
                CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Ready Active: %d",RW->Wheel[i].data.wheelId, RW->Wheel[i].data.active);
            } else {
                CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_ERROR, "RW ID %d Telecommand Error",RW->Wheel[i].data.wheelId);
                wheelStatus &= false;
                RW->Wheel[i].data.active = false;
            }
        }
    }
    CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_INFORMATION, "RW ADCS_HW_RW_Start(). Status %d, Active %d (may not be active yet)", Status, RW->Active);

    return Status;
}

int32_t ADCS_HW_RW_Stop(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    bool wheelStatus = true;
    ADCS_RW_Class* RW = (ADCS_RW_Class *) self->DeviceClassPtr;
    RW->Active = false;
    for (int i=0; i<RW->WheelCnt; i++) {
        cubewheel_close(&RW->Wheel[i]);
        wheelStatus  &= RW->Wheel[i].data.active;
    }
    if(wheelStatus == true) {
        Status = ADCS_HW_EXEC_FAILURE; // Set exec to failure if wheel is still active (shutdown failed...)
    } else {
        Status = ADCS_HW_EXEC_SUCCESS;
    }
    // Status = OS_CountSemDelete(RW->WakeUpSemaphore);
    CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_INFORMATION, "RW ADCS_HW_RW_Stop(). RW Status %d", Status);

    return Status;
} 

int32_t ADCS_HW_RW_Reset(ADCS_HW_Device_t *self) {
    // ADCS_RW_Class *RW = (ADCS_RW_Class *) self->DeviceClassPtr;
    int32_t ret;
    bool initStatus = ADCS_HW_RW_Init(self);
    CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_INFORMATION, "RW ADCS_HW_RW_Reset(). Reset -> Initialized. IMU Init Status %d", initStatus);
    if (initStatus == ADCS_HW_EXEC_SUCCESS) {
        ret = ADCS_HW_EXEC_SUCCESS;
    } else {
        ret = ADCS_HW_EXEC_FAILURE;
    }
    return ret;
}

int32_t ADCS_HW_RW_ResetStatus(ADCS_HW_Device_t *self) {
    CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG,
        "**** ADCS_HW_RW_ResetStatus()");

    ADCS_RW_Class *RW = (ADCS_RW_Class *) self->DeviceClassPtr;
    for (int i=0; i<RW->WheelCnt; i++) {
        cubewheel_reset_status(&RW->Wheel[i]);
    }
    return ADCS_HW_EXEC_SUCCESS;
}

int32_t ADCS_HW_RW_Update(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    ADCS_RW_Class *RW = (ADCS_RW_Class *) self->DeviceClassPtr;
    bool wheelStatus;
    int i;
    // uint32_t size;
    // ADCS_LOG_INFO("ADCS_HW_RW_Update() Started");
    if(RW->Active) {
        wheelStatus = true; // Start with good status for this update loop
        for (i = 0; i<RW->WheelCnt; i++) {
            wheelStatus &= cubewheel_get_periodic_tlm(&RW->Wheel[i]);
        }
    }
    if(!wheelStatus) {
        Status = ADCS_HW_EXEC_FAILURE;
    }
    // ADCS_LOG_INFO("ADCS_HW_RW_Update() Complete");
    return Status;

}

int32_t ADCS_HW_RW_TLM_Update(ADCS_HW_Device_t *self) {
    ADCS_RW_Class *RW = (ADCS_RW_Class *) self->DeviceClassPtr;
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    Status = ADCS_HW_MGR_UpdateCommonTlm(&RW->TlmPkt.DeviceTlm, self);
    for (int i=0; i<RW->WheelCnt; i++) {
        CFE_PSP_MemCpy(&RW->TlmPkt.WheelTlm[i], &RW->Wheel[i].data, sizeof(cubewheel_wheelData_t));
    }
    CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG, "****  ADCS_HW_RW_TLM_Update()");
    return Status;
}


/**********************************************************************/
/*  External Wheel Command Function                                   */
/**********************************************************************/

// int32_t ADCS_HW_RW_GetWheelTlm(ADCS_RW_Class *RW, uint8_t wheelId, uint8_t tlmId) {
//     ADCS_RW_TlmRequest_t tlmRequstObj;
//     tlmRequstObj.WheelId = wheelId;
//     tlmRequstObj.FrameId = tlmId;
//     TQUEUE_Push(&RW->TlmQueue, &tlmRequstObj, sizeof(ADCS_RW_TlmRequest_t));
//     int32_t Status;

//    if(cubewheel_get_tlm(&RW[wheelId].Wheel, tlmId)) {
//        Status = ADCS_HW_EXEC_SUCCESS;
//    } else {
//        Status = ADCS_HW_EXEC_FAILURE;
//    }
//     return Status;
// }

int32_t ADCS_HW_RW_GetIdentificationTlm(ADCS_RW_Class *RW) {
    int i;
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    bool wheelStatus;
    for (i=0; i<RW->WheelCnt; i++) {
        wheelStatus &= cubewheel_get_indentification_tlm(&RW->Wheel[i]);
    }
    if(!wheelStatus) {
        Status = ADCS_HW_EXEC_FAILURE;
    }
    return Status;
}

int32_t ADCS_HW_RW_WheelCmds(ADCS_RW_Class *RW, unsigned int wheelId, cubewheel_telecommand_t* wheelCmd) {
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    if (!cubewheel_send_tc(&RW->Wheel[wheelId], wheelCmd)) {
        Status = ADCS_HW_EXEC_FAILURE;
    }
    return Status;
}