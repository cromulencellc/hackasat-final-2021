#include "imu.h"

/*
** Global File Data
*/
ADCS_HW_Device_t *IMUDevice = NULL;

/*
** Local Function Prototypes
*/
int32_t ADCS_HW_IMU_Poll(ADCS_IMU_Class *IMU);

// IMU HW API
// static void ADCS_HW_IMU_ChildTask(void);

/*
** IMU Hal Functions
*/
int32_t ADCS_HW_IMU_Init(ADCS_HW_Device_t *self) {
    ADCS_IMU_Class* IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
    ImuData_Struct* imuConfTbl = (ImuData_Struct*) self->ConfTblPtr;
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    CFE_PSP_MemSet(&IMU->Data, 0, sizeof(IMU->Data));
    IMUDevice = self;
    bno08x_constructor(&IMU->Device);
    IMU->Device.numSensors = imuConfTbl->sensorCount;
    IMU->PollRateMs = imuConfTbl->pollRateMs;
    IMU->SensorCount = imuConfTbl->sensorCount;
    int i;
    for (i = 0; i<IMU->SensorCount; i++) {
        ImuSensor_Struct* sensorData = &imuConfTbl->imuSensors[i];
        bno08x_init_sensor_data(&IMU->Data[sensorData->sensorIdx], sensorData->sensorId, sensorData->sensorDataSize, sensorData->updateRate);
    }
    bno08x_set_reset_line(imuConfTbl->imuResetGpio.gpiochip, imuConfTbl->imuResetGpio.linenum);
    if (!IMU->Device.gpioReset.active) {
        Status = ADCS_HW_EXEC_FAILURE;
    }
    if(imuConfTbl->imuIntEnabled && Status == ADCS_HW_EXEC_SUCCESS) {
        bno08x_set_interrupt_line(imuConfTbl->imuIntGpio.gpiochip, imuConfTbl->imuIntGpio.linenum);
        if(!IMU->Device.gpioInterrupt.active) {
            Status =  ADCS_HW_EXEC_FAILURE;
        }
    }
    if (Status == ADCS_HW_EXEC_SUCCESS) {
        bno08x_init(imuConfTbl->i2cConfig.i2cPath, imuConfTbl->i2cConfig.i2cAddr, imuConfTbl->i2cConfig.i2cType);
        bno08x_override_sleep_fuction(OS_TaskDelay);
        bno08x_hardware_reset();
        if (!IMU->Device.i2c.active) {
            Status = ADCS_HW_EXEC_FAILURE;
        }
    }

    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_Init(). Status %d, IMU State %d, I2C State %d", Status, IMU->Device.active, IMU->Device.i2c.active);

    return Status;
}



int32_t ADCS_HW_IMU_Start(ADCS_HW_Device_t *self) {
    int32  CfeStatus = CFE_SUCCESS;
    int32_t Status = ADCS_HW_EXEC_FAILURE;
    boolean ImuStatus = TRUE;
    ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;

    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU Active: %d", IMU->Device.active);
    if (!IMU->Device.active) {
        if((ImuStatus = bno08x_open()) == true){
            CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU Driver Open Successful.");
        }
    }
    if (ImuStatus) {
        CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU Enable Dynamic Sensor Calibration");
        bno08x_enable_dynamic_calibration(true, true, true);
    }
    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU Active %d, IMU Status: %d, IMU Sensors Config Status %d", IMU->Device.active, ImuStatus, IMU->Device.sensorsConfigured);
    if(IMU->Device.active &&  !IMU->Device.sensorsConfigured && ImuStatus) {
        bno08x_setup_reports(IMU->Data, IMU->SensorCount);
        CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU Sensor Report Requests Completed.");
    }

    if(IMU->Device.active && IMU->Device.i2c.active && IMU->Device.sensorsConfigured && ImuStatus) {
        Status = ADCS_HW_EXEC_SUCCESS;
    }

    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_Start(). Start Status %d", Status);

    return CfeStatus;
}

int32_t ADCS_HW_IMU_Stop(ADCS_HW_Device_t *self) {
    ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
    int32_t Status;
    bno08x_close();
    // OS_CountSemDelete(IMU->WakeUpSemaphore);
    if (!IMU->Device.active) {
        Status = ADCS_HW_EXEC_SUCCESS;
    } else {
        Status = ADCS_HW_EXEC_FAILURE;
    }
    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_Stop(). IMU State %d, I2C State %d", IMU->Device.active, IMU->Device.i2c.active);

    return Status;
} 

int32_t ADCS_HW_IMU_Reset(ADCS_HW_Device_t *self) {
    ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_Reset(). Reset -> Initialized. . IMU State %d, I2C State %d", IMU->Device.active, IMU->Device.i2c.active);
    int32_t ret = ADCS_HW_IMU_Init(self);
    return ret;
}

int32_t ADCS_HW_IMU_ResetStatus(ADCS_HW_Device_t *self) {
    // ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
    bno08x_reset_status();
    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_ResetStatus()");
    return ADCS_HW_EXEC_SUCCESS;
}

int32_t ADCS_HW_IMU_Update(ADCS_HW_Device_t *self) {
    ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_DEBUG,
        "**** ADCS_HW_IMU_Update()");
    // ADCS_LOG_INFO("ADCS_HW_IMU_Update() Started");
    if(IMU->Device.active) {
        if(!bno08x_update()) {
            Status = ADCS_HW_EXEC_FAILURE;
            CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_ERROR, "ADCS_HW_IMU_Update() bno08x_update() failed");
        }
    }
    // ADCS_LOG_INFO("ADCS_HW_IMU_Update() Complete");
    return Status;
}

int32_t ADCS_HW_IMU_TLM_Update(ADCS_HW_Device_t *self) {
    ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
    int32_t Status;
    Status = ADCS_HW_MGR_UpdateCommonTlm(&IMU->TlmPkt.DeviceTlm, self);
    for (int i =0; i<IMU->Device.numSensors; i++) {
        IMU->TlmPkt.SensorData[i].sensorId = IMU->Data[i].sensorId;
        IMU->TlmPkt.SensorData[i].lastSensorSequence = IMU->Data[i].lastSequence;
        IMU->TlmPkt.SensorData[i].sensorSequenceMissedCnt = IMU->Data[i].sensorSequenceMissedCnt;
        IMU->TlmPkt.SensorData[i].sensorRecvCnt = IMU->Data[i].sensorRecvCnt;
        IMU->TlmPkt.SensorData[i].sensorSequence = IMU->Data[i].sensorValue.sequence;
        IMU->TlmPkt.SensorData[i].sensorStatus = IMU->Data[i].sensorValue.status;
        IMU->TlmPkt.SensorData[i].sensorTimestamp = IMU->Data[i].sensorValue.timestamp;
        IMU->TlmPkt.SensorData[i].sensorDelay = IMU->Data[i].sensorValue.delay;
        if (self->Status == HW_RUNNING) {
            CFE_PSP_MemCpy(&IMU->TlmPkt.SensorData[i].sensorData, &IMU->Data[i].sensorValue.un, IMU->Data[i].sensorValueUnSize);
        }
    }
    // CFE_PSP_MemCpy(&IMU->TlmPkt.SensorData, &IMU->Data, sizeof(IMU->Data));
    CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_DEBUG, "****  ADCS_HW_IMU_TLM_Update()");
    return Status;
}
