// #include "rw.h"

// /*
// ** Global File Data
// */
// #define IDENTIFICATION_FRAME_COUNT 2
// ADCS_RW_Class *RWDevice = NULL;
// static uint8_t identificationTlmFrameIds[IDENTIFICATION_FRAME_COUNT] = {TLM_WHEEL_IDENTIFICATION, TLM_WHEEL_EXTENDED_IDENTIFICATION}; 
// /*
// ** Local Function Prototypes
// */
// int32_t ADCS_HW_RW_Poll(ADCS_RW_Class *RW);
// static void ADCS_HW_RW_ChildTask(void);
// bool ADCS_HW_RW_ChildProcessTcCmd(ADCS_RW_Class* RW, ADCS_RW_Telecommand_t* RwCmd);
// bool ADCS_HW_RW_ChildProcessTlmCmd(ADCS_RW_Class* RW, ADCS_RW_TlmRequest_t* RwCmd);

// /*
// ** RW Hal Functions
// */
// int32_t ADCS_HW_RW_Init(ADCS_HW_Device_t *self) {
//     int32 Status = ADCS_HW_EXEC_SUCCESS;
//     ADCS_RW_Class* RW = (ADCS_RW_Class *) self->DeviceClassPtr;
//     RWDevice = RW; // Set file pointer for child task
//     WheelData_Struct* rwConfTbl = (WheelData_Struct*) self->ConfTblPtr;
//     RW->ControlMode = rwConfTbl->controlMode;
//     RW->WheelCnt = rwConfTbl->wheelCount;
//     RW->PollRateMs = rwConfTbl->pollRateMs;
//     bool wheelStatus = true;

//     printf("RW Init. Whl Cnt %d. Poll Rate Msec %d\n", RW->WheelCnt, RW->PollRateMs);

//     if (!RW->QueueSetup) {
//         TQUEUE_Setup(&RW->TelecommandQueue, &RW->TelecommandBuffer, sizeof(RW->TelecommandBuffer), "ADCS_RW_TC");
//         TQUEUE_Setup(&RW->TlmQueue, &RW->TlmRequestBuffer, sizeof(RW->TlmRequestBuffer), "ADCS_RW_TLM");
//         RW->QueueSetup = true;
//     }

//     if (Status == CFE_SUCCESS) {
//         for (int i = 0; i<RW->WheelCnt; i++) {
//                 cubewheel_constructor(&RW->Wheel[i], i);
//                 cubewheel_override_sleep_fuction(&RW->Wheel[i], &OS_TaskDelay);
//                 cubewheel_gpio_init(&RW->Wheel[i], rwConfTbl->wheelData[i].enableGpio.gpiochip, rwConfTbl->wheelData[i].enableGpio.linenum);
//                 cubewheel_hardware_reset(&RW->Wheel[i]);
//                 cubewheel_i2c_init(&RW->Wheel[i], rwConfTbl->wheelData[i].i2cConfig.i2cPath, rwConfTbl->wheelData[i].i2cConfig.i2cAddr, rwConfTbl->wheelData[i].i2cConfig.i2cType);
//                 wheelStatus  &= (RW->Wheel[i].gpioEnable.active && RW->Wheel[i].i2c.active);
//                 CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG,
//                     "RW %d Init. Status %d", i, wheelStatus);
//                 if(Status != OS_SUCCESS) {
//                     wheelStatus &= false;
//                     CFE_EVS_SendEvent(ADCS_HW_RW_ERROR_EID, CFE_EVS_ERROR,
//                         "RW %d Init Failure. Status %d", i, wheelStatus);
//                 }
//         }
//     } else {
//                 CFE_EVS_SendEvent(ADCS_HW_RW_ERROR_EID, CFE_EVS_ERROR,
//             "Failed to create %s semaphore. Status=0x%8X", ADCS_RW_CHILD_SEM_NAME, (int)Status);
//         wheelStatus = false;
//     }
    

//     if(!wheelStatus) {
//         CFE_EVS_SendEvent(ADCS_HW_RW_ERROR_EID, CFE_EVS_ERROR,
//             "RW Init Failure");
//         Status = ADCS_HW_EXEC_FAILURE;
//     } else {
//         Status = ADCS_HW_EXEC_SUCCESS;
//     }
    
//     return Status;
// }

// int32_t ADCS_HW_RW_Start(ADCS_HW_Device_t *self) {

//     int32  CfeStatus = CFE_SUCCESS;
//     int32_t Status = ADCS_HW_EXEC_SUCCESS;
//     boolean FailedToGetAppInfo = TRUE;
    
//     ADCS_RW_Class* RW = (ADCS_RW_Class *) self->DeviceClassPtr;
       
//     CfeStatus = OS_CountSemCreate(&RW->WakeUpSemaphore, ADCS_RW_CHILD_SEM_NAME, OS_SEM_EMPTY, 0);
//     if (CfeStatus == CFE_SUCCESS) {
//         if (!RW->ChildTaskActive) {
//             CfeStatus = CFE_ES_CreateChildTask(&RW->ChildTaskId, ADCS_RW_CHILD_NAME,
//                                                 ADCS_HW_RW_ChildTask, 0, ADCS_RW_STACK_SIZE,
//                                                 ADCS_RW_CHILD_PRIORITY, 0);      
//             if (CfeStatus != CFE_SUCCESS) {
                
//                 CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_ERROR,
//                                 "Failed to create child task %s. Status=0x%8X", ADCS_RW_CHILD_NAME, (int)CfeStatus);               
//                 Status = ADCS_HW_EXEC_FAILURE;
//             } else {
//                 // Give Child Task Semaphore to start
//                 CfeStatus = OS_CountSemGive(RW->WakeUpSemaphore);  
//             }
//         }

//     }
//     CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_INFORMATION, "RW ADCS_HW_RW_Start(). Status %d, Active %d (may not be active yet)", Status, RW->Active);

//     return Status;
// }

// int32_t ADCS_HW_RW_Stop(ADCS_HW_Device_t *self) {
//     int32_t Status = ADCS_HW_EXEC_SUCCESS;
//     bool wheelStatus = true;
//     ADCS_RW_Class* RW = (ADCS_RW_Class *) self->DeviceClassPtr;
//     RW->Active = false;
//     Status = OS_CountSemGive(RW->WakeUpSemaphore); /* Allow child to terminate gracefully */
//     for (int i=0; i<RW->WheelCnt; i++) {
//         cubewheel_close(&RW->Wheel[i]);
//         wheelStatus  &= RW->Wheel[i].data.active;
//     }
//     if(wheelStatus == true) {
//         Status = ADCS_HW_EXEC_FAILURE; // Set exec to failure if wheel is still active (shutdown failed...)
//     } else {
//         Status = ADCS_HW_EXEC_SUCCESS;
//     }
//     Status = OS_CountSemDelete(RW->WakeUpSemaphore);
//     CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_INFORMATION, "RW ADCS_HW_RW_Stop(). RW Status %d", Status);

//     return Status;
// } 

// int32_t ADCS_HW_RW_Reset(ADCS_HW_Device_t *self) {
//     ADCS_RW_Class *RW = (ADCS_RW_Class *) self->DeviceClassPtr;
//     int ret = OS_CountSemDelete(RW->WakeUpSemaphore);
//     if (ret != CFE_SUCCESS) {
//         CFE_EVS_SendEvent(ADCS_HW_RW_ERROR_EID, CFE_EVS_ERROR,
//             "Failed to delete %s semaphore. Status=0x%8X", ADCS_RW_CHILD_SEM_NAME, (int)ret);
//     }
//     bool initStatus = ADCS_HW_RW_Init(self);
//     CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_INFORMATION, "RW ADCS_HW_RW_Reset(). Reset -> Initialized. IMU Init Status %d", initStatus);
//     if (initStatus == ADCS_HW_EXEC_SUCCESS) {
//         ret = ADCS_HW_EXEC_SUCCESS;
//     } else {
//         ret = ADCS_HW_EXEC_FAILURE;
//     }
//     return ret;
// }

// int32_t ADCS_HW_RW_ResetStatus(ADCS_HW_Device_t *self) {
//     CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG,
//         "**** ADCS_HW_RW_ResetStatus()");

//     ADCS_RW_Class *RW = (ADCS_RW_Class *) self->DeviceClassPtr;
//     for (int i=0; i<RW->WheelCnt; i++) {
//         cubewheel_reset_status(&RW->Wheel[i]);
//     }
//     return ADCS_HW_EXEC_SUCCESS;
// }

// int32_t ADCS_HW_RW_Update(ADCS_HW_Device_t *self) {
//     return ADCS_HW_EXEC_SUCCESS;
// }

// int32_t ADCS_HW_RW_TLM_Update(ADCS_HW_Device_t *self) {
//     ADCS_RW_Class *RW = (ADCS_RW_Class *) self->DeviceClassPtr;
//     int32_t Status = ADCS_HW_EXEC_SUCCESS;
//     Status = ADCS_HW_MGR_UpdateCommonTlm(&RW->TlmPkt.DeviceTlm, self);
//     for (int i=0; i<RW->WheelCnt; i++) {
//         CFE_PSP_MemCpy(&RW->TlmPkt.WheelTlm[i], &RW->Wheel[i].data, sizeof(cubewheel_wheelData_t));
//     }
//     CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG, "****  ADCS_HW_RW_TLM_Update()");
//     return Status;
// }

// bool ADCS_HW_RW_ChildProcessTcCmd(ADCS_RW_Class* RW, ADCS_RW_Telecommand_t* RwCmd) {
//     CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG, "RW Command Processed");
//     bool status = cubewheel_send_tc(&RW->Wheel[RwCmd->WheelId], RwCmd->WheelCmd);
//     return status;
// }


// bool ADCS_HW_RW_ChildProcessTlmCmd(ADCS_RW_Class* RW, ADCS_RW_TlmRequest_t* RwCmd) {
//     CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG, "RW Command Processed");
//     bool status = cubewheel_get_tlm(&RW->Wheel[RwCmd->WheelId], RwCmd->FrameId);
//     return status;
// }

// int32_t ADCS_HW_RW_Poll(ADCS_RW_Class *RW) {
//     ADCS_RW_Telecommand_t tcCmd;
//     ADCS_RW_TlmRequest_t tlmCmd;
//     bool wheelStatus;
//     int i;
//     uint32_t size;
//     while(RW->Active) {
//         wheelStatus = true; // Start with good status for this update loop
//         for (i = 0; i<RW->WheelCnt; i++) {
//             wheelStatus &= cubewheel_get_periodic_tlm(&RW->Wheel[i]);
//         }
//         while(true) {
//             size = TQUEUE_Pop(&RW->TlmQueue, (uint8_t* ) &tlmCmd, sizeof(ADCS_RW_TlmRequest_t));
//             if(size == 0) {
//                 break;
//             }
//             wheelStatus &= ADCS_HW_RW_ChildProcessTlmCmd(RW, &tlmCmd);
//         }
//         while(true) {
//             size = TQUEUE_Pop(&RW->TelecommandQueue, (uint8_t*) &tcCmd, sizeof(ADCS_RW_Telecommand_t));
//             if (size == 0) {
//                 break;
//             }
//             wheelStatus &= ADCS_HW_RW_ChildProcessTcCmd(RW, &tcCmd);
//         }

//         CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG, "RW Update. Status: %d", wheelStatus);
//         OS_TaskDelay(RW->PollRateMs);
//     }
//     return wheelStatus;
// }



// static void ADCS_HW_RW_ChildTask(void) {
//     int32 CfeStatus;
//     CfeStatus = CFE_ES_RegisterChildTask();
//     ADCS_RW_Class* RW = RWDevice;

//    if (CfeStatus == CFE_SUCCESS) {
//         CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW child task initialization complete");

//         CFE_EVS_SendEvent(ADCS_HW_RW_DEBUG_EID, CFE_EVS_DEBUG,
//             "\n\n**** RW Child Task. Waiting for semaphore\n");    
//         CfeStatus = OS_CountSemTake(RW->WakeUpSemaphore); /* Pend until parent app gives semaphore */
//         if ((CfeStatus == CFE_SUCCESS)) {
//             bool tcStatus;
//             bool wheelStatus = true;
//             int i;
//             for (i=0; i<RW->WheelCnt; i++) {
//                 cubewheel_i2c_begin(&RW->Wheel[i]);
//                 wheelStatus  &= RW->Wheel[i].data.active;
//                 CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Active: %d",RW->Wheel[i].data.wheelId, RW->Wheel[i].data.active);
//             }
//             if(wheelStatus) {
//                 for (i=0; i<RW->WheelCnt; i++) {
//                     tcStatus = cubewheel_tc_set_control_mode(&RW->Wheel[i], RW->ControlMode);
//                     if(RW->ControlMode == WHEEL_MODE_USE_SPEED_CONTROL && tcStatus) {
//                         tcStatus = cubewheel_tc_set_reference_speed(&RW->Wheel[i], (float) 0.0);
//                         CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Init Speed Control (0 rpm). TC Status %d",RW->Wheel[i].data.wheelId, tcStatus);
//                     } else if (RW->ControlMode == WHEEL_MODE_DUTY_CYCLE_CONTROL && tcStatus) 
//                     {
//                         tcStatus = cubewheel_tc_set_duty_cycle(&RW->Wheel[i], 0);
//                         CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Init Duty-Cycle Control (0%%). TC Staus %d",RW->Wheel[i].data.wheelId, tcStatus);
//                     } else if ((RW->ControlMode == WHEEL_MODE_IDLE || RW->ControlMode == WHEEL_MODE_NO_CONTROL) && tcStatus)
//                     {
//                        CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Init IDLE or NO-CONTROL. TC Staus %d",RW->Wheel[i].data.wheelId, tcStatus);
//                     } 
//                     if (tcStatus) {
//                         RW->Active = true;
//                         CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW ID %d Ready Active: %d",RW->Wheel[i].data.wheelId, RW->Wheel[i].data.active);
//                     } else {
//                         CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_ERROR, "RW ID %d Telecommand Error",RW->Wheel[i].data.wheelId, RW->Wheel[i].data.active);
//                         wheelStatus &= false;
//                         RW->Wheel[i].data.active = false;
//                     }
//                 }
//             }
//             if(wheelStatus) {
//                 CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_INFORMATION, "RW Starting Update Poll Loop, Status: %d, RW Active %d", wheelStatus, RW->Active);
//                 CfeStatus = ADCS_HW_RW_Poll(RW);
//             } else {
//                 CfeStatus = ADCS_HW_EXEC_FAILURE;
//                 CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_ERROR, "RW Device Start Failed, Status: %d, RW Active %d", wheelStatus, RW->Active);
//             }
//       } /* End if valid semaphore */
//    } /* End if CFE_ES_RegisterChildTask() successful */
//    else {
       
//       CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_ERROR, 
//                         "ADCS_HW call to CFE_ES_RegisterChildTask() failed, Status=%d",
//                         (int)CfeStatus);
//    }

//     CFE_EVS_SendEvent(ADCS_HW_RW_CHILD_TASK_EID, CFE_EVS_ERROR, "Exiting RW Child Task");
//     // RW->WakeUpSemaphore = ADCS_RW_SEM_INVALID;  /* Prevent parent from invoking the child task */
   
//     CFE_ES_ExitChildTask();  /* Clean-up system resources */
//     RW->ChildTaskActive = false;
// }

// /**********************************************************************/
// /*  External Wheel Command Function                                   */
// /**********************************************************************/

// int32_t ADCS_HW_RW_GetWheelTlm(ADCS_RW_Class *RW, uint8_t wheelId, uint8_t tlmId) {
//     ADCS_RW_TlmRequest_t tlmRequstObj;
//     tlmRequstObj.WheelId = wheelId;
//     tlmRequstObj.FrameId = tlmId;
//     TQUEUE_Push(&RW->TlmQueue, &tlmRequstObj, sizeof(ADCS_RW_TlmRequest_t));
//     return ADCS_HW_EXEC_SUCCESS;
// }

// int32_t ADCS_HW_RW_GetIdentificationTlm(ADCS_RW_Class *RW) {
//     int i, j; 
//     int tlmFrameCount = 0;
//     ADCS_RW_TlmRequest_t getTlmCmds[RW->WheelCnt*IDENTIFICATION_FRAME_COUNT];
//     CFE_PSP_MemSet(getTlmCmds, 0, sizeof(ADCS_RW_TlmRequest_t)*RW->WheelCnt);
//     for (i=0; i<RW->WheelCnt; i++) {
//         for (j=0; j<IDENTIFICATION_FRAME_COUNT; j++) {
//             getTlmCmds[tlmFrameCount].WheelId = RW->Wheel[i].data.wheelId;
//             getTlmCmds[tlmFrameCount].FrameId = identificationTlmFrameIds[j];
//             tlmFrameCount++;
//         }
//     }
//     TQUEUE_Push(&RW->TlmQueue, &getTlmCmds, sizeof(ADCS_RW_TlmRequest_t)*RW->WheelCnt);
//     return ADCS_HW_EXEC_SUCCESS;
// }

// int32_t ADCS_HW_RW_PushWheelCmd(ADCS_RW_Class *RW, unsigned int wheelId, unsigned int cmdCount, cubewheel_telecommand_ptr_t* wheelCmds) {
//     ADCS_RW_Telecommand_t wheelCmdList[cmdCount];
//     for (int i=0; i<cmdCount; i++) {
//         wheelCmdList[i].WheelId = wheelId;
//         wheelCmdList[i].WheelCmd = wheelCmds[i];
//     }
//     TQUEUE_Push(&RW->TelecommandQueue, &wheelCmdList, sizeof(ADCS_RW_Telecommand_t)*cmdCount);
//     return ADCS_HW_EXEC_SUCCESS;
// }