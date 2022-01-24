// #include "imu.h"

// /*
// ** Global File Data
// */
// ADCS_HW_Device_t *IMUDevice = NULL;

// /*
// ** Local Function Prototypes
// */
// int32_t ADCS_HW_IMU_Poll(ADCS_IMU_Class *IMU);

// // IMU HW API
// static void ADCS_HW_IMU_ChildTask(void);

// /*
// ** IMU Hal Functions
// */
// int32_t ADCS_HW_IMU_Init(ADCS_HW_Device_t *self) {
//     ADCS_IMU_Class* IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
//     ImuData_Struct* imuConfTbl = (ImuData_Struct*) self->ConfTblPtr;
//     int32_t Status = ADCS_HW_EXEC_SUCCESS;
//     CFE_PSP_MemSet(&IMU->Data, 0, sizeof(IMU->Data));
//     IMUDevice = self;
//     bno08x_constructor(&IMU->Device);
//     IMU->Device.numSensors = imuConfTbl->sensorCount;
//     IMU->PollRateMs = imuConfTbl->pollRateMs;
//     IMU->SensorCount = imuConfTbl->sensorCount;
//     int i;
//     for (i = 0; i<IMU->SensorCount; i++) {
//         ImuSensor_Struct* sensorData = &imuConfTbl->imuSensors[i];
//         bno08x_init_sensor_data(&IMU->Data[sensorData->sensorIdx], sensorData->sensorId, sensorData->sensorDataSize, sensorData->updateRate);
//     }
//     bno08x_set_reset_line(imuConfTbl->imuResetGpio.gpiochip, imuConfTbl->imuResetGpio.linenum);
//     if (!IMU->Device.gpioReset.active) {
//         Status = ADCS_HW_EXEC_FAILURE;
//     }
//     if(imuConfTbl->imuIntEnabled && Status == ADCS_HW_EXEC_SUCCESS) {
//         bno08x_set_interrupt_line(imuConfTbl->imuIntGpio.gpiochip, imuConfTbl->imuIntGpio.linenum);
//         if(!IMU->Device.gpioInterrupt.active) {
//             Status =  ADCS_HW_EXEC_FAILURE;
//         }
//     }
//     if (Status == ADCS_HW_EXEC_SUCCESS) {
//         bno08x_init(imuConfTbl->i2cConfig.i2cPath, imuConfTbl->i2cConfig.i2cAddr, imuConfTbl->i2cConfig.i2cType);
//         bno08x_override_sleep_fuction(OS_TaskDelay);
//         bno08x_hardware_reset();
//         if (!IMU->Device.i2c.active) {
//             Status = ADCS_HW_EXEC_FAILURE;
//         }
//     }

//     CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_Init(). Status %d, IMU State %d, I2C State %d", Status, IMU->Device.active, IMU->Device.i2c.active);

//     return Status;
// }



// int32_t ADCS_HW_IMU_Start(ADCS_HW_Device_t *self) {
//     int32  CfeStatus = CFE_SUCCESS;
//     int32_t Status = ADCS_HW_EXEC_SUCCESS;
//     boolean FailedToGetAppInfo = TRUE;
//     ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
//     CfeStatus = OS_CountSemCreate(&IMU->WakeUpSemaphore, ADCS_IMU_CHILD_SEM_NAME, OS_SEM_EMPTY, 0);

//     if (CfeStatus != CFE_SUCCESS) {
//         CFE_EVS_SendEvent(ADCS_HW_IMU_ERROR_EID, CFE_EVS_ERROR,
//             "Failed to create %s semaphore. Status=0x%8X", ADCS_IMU_CHILD_SEM_NAME, (int)CfeStatus);
//     }
//     if (CfeStatus == CFE_SUCCESS) {
//         if (!IMU->ChildTaskActive) {
//             CfeStatus = CFE_ES_CreateChildTask(&IMU->ChildTaskId, ADCS_IMU_CHILD_NAME,
//                                                 ADCS_HW_IMU_ChildTask, 0, ADCS_IMU_STACK_SIZE,
//                                                 ADCS_IMU_CHILD_PRIORITY, 0);      
//             if (CfeStatus != CFE_SUCCESS) {
                
//                 CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_ERROR,
//                                 "Failed to create child task %s. Status=0x%8X", ADCS_IMU_CHILD_NAME, (int)CfeStatus);               
//                 Status = ADCS_HW_EXEC_FAILURE;
//             } else {
//                 // Give Child Task Semaphore to start
//                 CfeStatus = OS_CountSemGive(IMU->WakeUpSemaphore);  
//             }
//         }
//     }
//     CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_Start(). Status %d, IMU State %d, I2C State %d", Status, IMU->Device.active, IMU->Device.i2c.active);

//     return CfeStatus;
// }

// int32_t ADCS_HW_IMU_Stop(ADCS_HW_Device_t *self) {
//     ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
//     int32_t Status;
//     if (IMU->InterruptsEnabled) {
//         bno08x_disable_interrupt_updates();
//     }
//     bno08x_close();
//     Status= OS_CountSemGive(IMU->WakeUpSemaphore); /* Allow child to terminate gracefully */
//     // OS_CountSemDelete(IMU->WakeUpSemaphore);
//     if (!IMU->Device.active) {
//         Status = ADCS_HW_EXEC_SUCCESS;
//     } else {
//         Status = ADCS_HW_EXEC_FAILURE;
//     }
//     CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_Stop(). IMU State %d, I2C State %d", IMU->Device.active, IMU->Device.i2c.active);

//     return Status;
// } 

// int32_t ADCS_HW_IMU_Reset(ADCS_HW_Device_t *self) {
//     ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
//     int ret = OS_CountSemDelete(IMU->WakeUpSemaphore);
//     if (ret != CFE_SUCCESS) {
//         CFE_EVS_SendEvent(ADCS_HW_IMU_ERROR_EID, CFE_EVS_ERROR,
//             "Failed to delete %s semaphore. Status=0x%8X", ADCS_IMU_CHILD_SEM_NAME, (int)ret);
//     }
//     CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_Reset(). Reset -> Initialized. . IMU State %d, I2C State %d", IMU->Device.active, IMU->Device.i2c.active);
//     ret = ADCS_HW_IMU_Init(self);
//     return ret;
// }

// int32_t ADCS_HW_IMU_ResetStatus(ADCS_HW_Device_t *self) {
//     ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
//     bno08x_reset_status();
//     CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_INFORMATION, "IMU ADCS_HW_IMU_ResetStatus()");
//     return ADCS_HW_EXEC_SUCCESS;
// }

// int32_t ADCS_HW_IMU_Update(ADCS_HW_Device_t *self) {
//     // ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
//     int32_t Status = ADCS_HW_EXEC_SUCCESS;
//     CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_DEBUG,
//         "**** ADCS_HW_IMU_Update()");
//     return Status;
// }

// int32_t ADCS_HW_IMU_TLM_Update(ADCS_HW_Device_t *self) {
//     ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) self->DeviceClassPtr;
//     int32_t Status;
//     Status = ADCS_HW_MGR_UpdateCommonTlm(&IMU->TlmPkt.DeviceTlm, self);
//     for (int i =0; i<IMU->Device.numSensors; i++) {
//         IMU->TlmPkt.SensorData[i].sensorId = IMU->Data[i].sensorId;
//         IMU->TlmPkt.SensorData[i].lastSensorSequence = IMU->Data[i].lastSequence;
//         IMU->TlmPkt.SensorData[i].sensorSequenceMissedCnt = IMU->Data[i].sensorSequenceMissedCnt;
//         IMU->TlmPkt.SensorData[i].sensorRecvCnt = IMU->Data[i].sensorRecvCnt;
//         IMU->TlmPkt.SensorData[i].sensorSequence = IMU->Data[i].sensorValue.sequence;
//         IMU->TlmPkt.SensorData[i].sensorStatus = IMU->Data[i].sensorValue.status;
//         IMU->TlmPkt.SensorData[i].sensorTimestamp = IMU->Data[i].sensorValue.timestamp;
//         IMU->TlmPkt.SensorData[i].sensorDelay = IMU->Data[i].sensorValue.delay;
//         if (self->Status == HW_RUNNING) {
//             CFE_PSP_MemCpy(&IMU->TlmPkt.SensorData[i].sensorData, &IMU->Data[i].sensorValue.un, IMU->Data[i].sensorValueUnSize);
//         }
//     }
//     // CFE_PSP_MemCpy(&IMU->TlmPkt.SensorData, &IMU->Data, sizeof(IMU->Data));
//     CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_DEBUG, "****  ADCS_HW_IMU_TLM_Update()");
//     return Status;
// }

// int32_t ADCS_HW_IMU_Poll(ADCS_IMU_Class *IMU) {
//     int Status = CFE_SUCCESS;
//     bool imuStatus;
//     while(IMU->Device.active) {
//         imuStatus = bno08x_update();
//         CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_DEBUG, "IMU Sensor Update. Status: %d", imuStatus);
//         OS_TaskDelay(IMU->PollRateMs);
//     }
//     return Status;
// }

// static void ADCS_HW_IMU_ChildTask(void) {
//     int32 CfeStatus;
//     bool imuStatus;
//     int32 updateFailureCount = 0;
//     CfeStatus = CFE_ES_RegisterChildTask();

//     ADCS_IMU_Class *IMU = (ADCS_IMU_Class *) IMUDevice->DeviceClassPtr;

//    if (CfeStatus == CFE_SUCCESS) {
//         CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_INFORMATION, "IMU child task initialization complete");

//         CFE_EVS_SendEvent(ADCS_HW_IMU_DEBUG_EID, CFE_EVS_DEBUG,
//             "\n\n**** IMU Child Task (%d) Waiting for semaphore\n",
//             IMUDevice->DeviceUpdateCnt);    
//         CfeStatus = OS_CountSemTake(IMU->WakeUpSemaphore); /* Pend until parent app gives semaphore */        updateFailureCount = 0;
//         if ((CfeStatus == CFE_SUCCESS)) {
            
//             CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_INFORMATION, "IMU Active: %d", IMU->Device.active);
//             if (!IMU->Device.active) {
//                 if(bno08x_open()){
//                     CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_INFORMATION, "IMU Driver Open Successful.");
//                 }
//             }
//             CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_INFORMATION, "IMU Active %d, IMU Sensors Config Status %d", IMU->Device.active, IMU->Device.sensorsConfigured);
//             if(IMU->Device.active &&  !IMU->Device.sensorsConfigured) {
//                 bno08x_setup_reports(IMU->Data, IMU->SensorCount);
//                 CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_INFORMATION, "IMU Sensor Report Requests Completed.");
//             }
//             if(IMU->Device.active  && IMU->Device.sensorsConfigured && IMU->InterruptsEnabled) {
//                 bno08x_enable_interrupt_updates(1.0); // Blocks thread until interuptUpdatesEnabled = false
//             } else if(IMU->Device.active  && IMU->Device.sensorsConfigured && !IMU->InterruptsEnabled) {
//                 CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_INFORMATION, "IMU Starting Time Based Sensor Polling, Status: %d, IMU Active %d", CfeStatus, IMU->Device.active);
//                 CfeStatus = ADCS_HW_IMU_Poll(IMU);
//             } else {
//                 CfeStatus = ADCS_HW_EXEC_FAILURE;
//                 CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_ERROR, "IMU Device Update Start Failed, Status: %d, IMU Active %d", CfeStatus, IMU->Device.active);
//             }
//       } /* End if valid semaphore */
//    } /* End if CFE_ES_RegisterChildTask() successful */
//    else {
       
//       CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_ERROR, 
//                         "ADCS_HW call to CFE_ES_RegisterChildTask() failed, Status=%d",
//                         (int)CfeStatus);
//    }

//     CFE_EVS_SendEvent(ADCS_HW_IMU_CHILD_TASK_EID, CFE_EVS_ERROR, "Exiting IMU Child Task");
//     // IMU->WakeUpSemaphore = ADCS_IMU_SEM_INVALID;  /* Prevent parent from invoking the child task */
   
//     CFE_ES_ExitChildTask();  /* Clean-up system resources */
//     IMU->ChildTaskActive = false;
// }