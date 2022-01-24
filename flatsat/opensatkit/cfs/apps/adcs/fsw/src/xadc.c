#include "xadc.h"

/*
** Local Function Prototypes
*/

/*
** XADC Hal Functions
*/
int32_t ADCS_HW_XADC_Init(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    ADCS_XADC_Class* XADC = (ADCS_XADC_Class *) self->DeviceClassPtr;
    XadcData_Struct* xadcConfTbl = (XadcData_Struct*) self->ConfTblPtr;
    bool xadcStatus;
    xadc_constructor(&XADC->Device);
    xadcStatus = xadc_init(xadcConfTbl->iioDevice, xadcConfTbl->iioTrigger, xadcConfTbl->sampleRate, false);
    CFE_EVS_SendEvent(ADCS_HW_XADC_DEBUG_EID, CFE_EVS_INFORMATION,
            "XADC Post-Init. Status %d", xadcStatus);
    if(xadcStatus) {
        xadcStatus = xadc_setup_channels(xadcConfTbl->channelCount, xadcConfTbl->channelNames);
        for (int i=0; i<xadcConfTbl->channelCount; i++) {
            xadc_set_channel_has_offset(i, xadcConfTbl->channelHasOffset[i]);
        }
    }
    if (xadcStatus && XADC->Device.active) {
        Status = ADCS_HW_EXEC_SUCCESS;
    } else {
        CFE_EVS_SendEvent(ADCS_HW_XADC_ERROR_EID, CFE_EVS_ERROR,
            "XADC Init Failure. Status %d", xadcStatus);
        Status = ADCS_HW_EXEC_FAILURE;

    }
    CFE_EVS_SendEvent(ADCS_HW_XADC_ERROR_EID, CFE_EVS_INFORMATION,
        "XADC ADCS_HW_XADC_Init(). Status %d", Status);
    return Status;
}


int32_t ADCS_HW_XADC_Start(ADCS_HW_Device_t *self) {
    int32  Status = ADCS_HW_EXEC_SUCCESS;
    CFE_EVS_SendEvent(ADCS_HW_XADC_DEBUG_EID, CFE_EVS_INFORMATION, "XADC -> RUNNING");
    return Status;
}

int32_t ADCS_HW_XADC_Stop(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    ADCS_XADC_Class* XADC = (ADCS_XADC_Class *) self->DeviceClassPtr;
    xadc_stop_channels();
    xadc_shutdown();
    CFE_EVS_SendEvent(ADCS_HW_XADC_DEBUG_EID, CFE_EVS_INFORMATION,
            "ADCS_HW_XADC_Stop(). Active %d", XADC->Device.active);
    return Status;
} 

int32_t ADCS_HW_XADC_Reset(ADCS_HW_Device_t *self) {
    ADCS_XADC_Class* XADC = (ADCS_XADC_Class *) self->DeviceClassPtr;
    CFE_EVS_SendEvent(ADCS_HW_XADC_DEBUG_EID, CFE_EVS_INFORMATION,
            "ADCS_HW_XADC_Reset(). Reset->Init. Active %d", XADC->Device.active);
    return ADCS_HW_XADC_Init(self);
}

int32_t ADCS_HW_XADC_ResetStatus(ADCS_HW_Device_t *self) {
    // TODO - Add any XADC specific status
    // ADCS_XADC_Class* XADC = (ADCS_XADC_Class *) self->DeviceClassPtr;
    CFE_EVS_SendEvent(ADCS_HW_XADC_DEBUG_EID, CFE_EVS_INFORMATION,
            "ADCS_HW_XADC_ResetStatus()");
    return ADCS_HW_EXEC_SUCCESS;
}

int32_t ADCS_HW_XADC_Update(ADCS_HW_Device_t *self) {
    ADCS_XADC_Class* XADC = (ADCS_XADC_Class *) self->DeviceClassPtr;
    CFE_EVS_SendEvent(ADCS_HW_XADC_DEBUG_EID, CFE_EVS_DEBUG,
            "ADCS_HW_XADC_Update()");
    if(XADC->Device.active) {
        xadc_update();
    }
    return ADCS_HW_EXEC_SUCCESS;
}

int32_t ADCS_HW_XADC_TLM_Update(ADCS_HW_Device_t *self) {
    int32_t Status;
    ADCS_XADC_Class* XADC = (ADCS_XADC_Class *) self->DeviceClassPtr;
    Status = ADCS_HW_MGR_UpdateCommonTlm(&XADC->TlmPkt.DeviceTlm, self);
    for (int i=0; i<XADC->Device.channel_count; i++) {
        XADC->TlmPkt.XadcTlmData[i].RawValue = XADC->Device.channels[i].raw_value;
        XADC->TlmPkt.XadcTlmData[i].ConvertedValue = XADC->Device.channels[i].converted;
        XADC->TlmPkt.XadcTlmData[i].Id = XADC->Device.channels[i].channel_index;
    }
    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_DEBUG, "****  ADCS_HW_XADC_TLM_Update()");
    return Status;
}
