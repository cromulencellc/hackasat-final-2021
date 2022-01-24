#include "mtr.h"

/*
** Local Function Prototypes
*/

/*
** MTR Hal Functions
*/
int32_t ADCS_HW_MTR_Init(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    ADCS_MTR_Class* MTR = (ADCS_MTR_Class *) self->DeviceClassPtr;
    MtrData_Struct* mtrConfTbl = (MtrData_Struct*) self->ConfTblPtr;
    CFE_PSP_MemSet(&MTR->MtrState, 0, sizeof(MTR->MtrState));
    MTR->MtrCount = mtrConfTbl->channelCount;
    int i;
    for (i=0; i<MTR->MtrCount; i++) {
        mtr_constructor(mtrConfTbl->mtrChannels[i].channelName, &MTR->Mtr[i]);
        Status = mtr_init(mtrConfTbl->mtrChannels[i].channel_p.gpiochip,
                 mtrConfTbl->mtrChannels[i].channel_p.linenum,
                 mtrConfTbl->mtrChannels[i].channel_n.gpiochip,
                 mtrConfTbl->mtrChannels[i].channel_n.linenum,
                 &MTR->Mtr[i]);
        CFE_EVS_SendEvent(ADCS_HW_MTR_DEBUG_EID, CFE_EVS_INFORMATION,
            "Torque Rod %s Init. Status %d, Active %d, State %d", MTR->Mtr[i].device_name, Status, MTR->Mtr[i].active, MTR->Mtr[i].state);
        if (Status == MTR_SUCCESS) {
            Status = ADCS_HW_EXEC_SUCCESS;
        } else {
            CFE_EVS_SendEvent(ADCS_HW_MTR_ERROR_EID, CFE_EVS_ERROR,
                "Torque Rod %s Init Failure. Status %d", MTR->Mtr[i].device_name, Status);
            return ADCS_HW_EXEC_FAILURE;

        }
    }
    return Status;
}


int32_t ADCS_HW_MTR_Start(ADCS_HW_Device_t *self) {
    int32  Status = ADCS_HW_EXEC_SUCCESS;
    CFE_EVS_SendEvent(ADCS_HW_MTR_DEBUG_EID, CFE_EVS_INFORMATION, "Torque Rod -> RUNNING");
    return Status;
}

int32_t ADCS_HW_MTR_Stop(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    ADCS_MTR_Class* MTR = (ADCS_MTR_Class *) self->DeviceClassPtr;
    for (int i=0; i<MTR->MtrCount; i++) {
        mtr_close(&MTR->Mtr[i]);
        CFE_EVS_SendEvent(ADCS_HW_MTR_DEBUG_EID, CFE_EVS_INFORMATION,
            "Torque Rod %s Closed. Active %d, State %d", MTR->Mtr[i].device_name, MTR->Mtr[i].active, MTR->Mtr[i].state);
    }

    return Status;
} 

int32_t ADCS_HW_MTR_Reset(ADCS_HW_Device_t *self) {
    ADCS_MTR_Class* MTR = (ADCS_MTR_Class *) self->DeviceClassPtr;
    for (int i=0; i<MTR->MtrCount; i++) {
        MTR->MtrResetCnt[i]++;
        mtr_reset(&MTR->Mtr[i]);
        CFE_EVS_SendEvent(ADCS_HW_MTR_DEBUG_EID, CFE_EVS_INFORMATION,
            "Torque Rod %s Reset. Active %d, State %d", MTR->Mtr[i].device_name, MTR->Mtr[i].active, MTR->Mtr[i].state);
    }
    return ADCS_HW_EXEC_SUCCESS;
}

int32_t ADCS_HW_MTR_ResetStatus(ADCS_HW_Device_t *self) {
    // TODO - Add any MTR specific status
    ADCS_MTR_Class* MTR = (ADCS_MTR_Class *) self->DeviceClassPtr;
    for (int i=0; i<MTR->MtrCount; i++) {
        MTR->MtrStateUpdateCnt[i] = 0;
        MTR->MtrResetCnt[i] = 0;
        CFE_EVS_SendEvent(ADCS_HW_MTR_DEBUG_EID, CFE_EVS_INFORMATION,
            "Torque Rod %s Reset Status. Active %d, State %d", MTR->Mtr[i].device_name, MTR->Mtr[i].active, MTR->Mtr[i].state);
    }
    return ADCS_HW_EXEC_SUCCESS;
}

int32_t ADCS_HW_MTR_Update(ADCS_HW_Device_t *self) {
    // ADCS_MTR_Class* MTR = (ADCS_MTR_Class *) self->DeviceClassPtr;
    return ADCS_HW_EXEC_SUCCESS;
}

int32_t ADCS_HW_MTR_TLM_Update(ADCS_HW_Device_t *self) {
    int32_t Status;
    ADCS_MTR_Class* MTR = (ADCS_MTR_Class *) self->DeviceClassPtr;
    for (int i=0; i<MTR->MtrCount; i++) {
        MTR->TlmPkt.MtrTlmData[i].Id = i;
        MTR->TlmPkt.MtrTlmData[i].State = MTR->Mtr[i].state;
        MTR->TlmPkt.MtrTlmData[i].GpioPState = MTR->Mtr[i].gpio_p.value;
        MTR->TlmPkt.MtrTlmData[i].GpioNState = MTR->Mtr[i].gpio_n.value;
        MTR->TlmPkt.MtrTlmData[i].StateUpdateCount = MTR->MtrStateUpdateCnt[i];
        MTR->TlmPkt.MtrTlmData[i].ResetCount = MTR->MtrResetCnt[i];
    }
    Status = ADCS_HW_MGR_UpdateCommonTlm(&MTR->TlmPkt.DeviceTlm, self);
    // CFE_PSP_MemCpy(&IMU->TlmPkt.SensorData, &IMU->Data, sizeof(IMU->Data));
    CFE_EVS_SendEvent(ADCS_HW_MTR_DEBUG_EID, CFE_EVS_DEBUG, "****  ADCS_HW_MTR_TLM_Update()");
    return Status;
}
