#include "fss.h"

/*
** Local Function Prototypes
*/

#if _FSS_CHALLENGE_ == 1
FSS_Cal ADCS_Fss_Calibration;
#endif
/*
** FSS Hal Functions
*/
int32_t ADCS_HW_FSS_Init(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_FAILURE;

    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
        ADCS_FSS_Class* FSS = (ADCS_FSS_Class *) self->DeviceClassPtr;

        CFE_PSP_MemSet(&FSS->data, 0, sizeof(FSS_Sensor_Data));
        FSS->Cal = &ADCS_Fss_Calibration;

        Status = ADCS_HW_EXEC_SUCCESS;

        FSS->Active = HW_INITIALIZED;

        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "Fine Sun Sensor Init. Active: %d", FSS->Active);

        if (Status == ADCS_HW_EXEC_SUCCESS) { // TODO: change this success to something meaningful for FSS
            Status = ADCS_HW_EXEC_SUCCESS;
        } else {
            CFE_EVS_SendEvent(ADCS_HW_FSS_ERROR_EID, CFE_EVS_ERROR,
                "Fine Sun Sensor Init Failure. Active: %d", FSS->Active);
            return ADCS_HW_EXEC_FAILURE;
        }

        return Status;
    #endif // _FSS_CHALLENGE_
}

int32_t ADCS_HW_FSS_Dump(ADCS_HW_Device_t *self){
    int32 Status = ADCS_HW_EXEC_SUCCESS;

    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
        ADCS_FSS_Class* FSS = (ADCS_FSS_Class *) self->DeviceClassPtr;
        int32 FileHandle;

        FileHandle = OS_creat("/cf/FSS_dump.bin", OS_WRITE_ONLY);

        int32 first_half = sizeof(FSS_Sensor_Data) + sizeof(ADCS_HW_FSS_TlmSensorPkt) + 1;
        int32 sec_half   = ADCS_FSS_CLASS_LEN - sizeof(FSS_Sensor_Data) - sizeof(ADCS_HW_FSS_TlmSensorPkt) - 1;

        if (FileHandle >= OS_FS_SUCCESS){
            OS_write(FileHandle, FSS, first_half);
            OS_write(FileHandle, FSS->ConfTblPtr, sizeof(FssData_Struct));
            OS_write(FileHandle, FSS, sec_half);
            OS_close(FileHandle);
        }
        else{
            CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION, "FSS dump failed");
            Status = ADCS_HW_EXEC_FAILURE;
        }

        return Status;
    #endif // _FSS_CHALLENGE
}

int32_t ADCS_HW_FSS_Start(ADCS_HW_Device_t *self) {
    int32  Status = ADCS_HW_EXEC_SUCCESS;
    
    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
        ADCS_FSS_Class* FSS = (ADCS_FSS_Class *) self->DeviceClassPtr;

        FSS->Active = HW_RUNNING;

        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION, "Fine Sun Sensor -> RUNNING");
    
        return Status;
    #endif // _FSS_CHALLENGE_
}

int32_t ADCS_HW_FSS_Stop(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_SUCCESS;

    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
        ADCS_FSS_Class* FSS = (ADCS_FSS_Class *) self->DeviceClassPtr;
        
        if(FSS->Active > HW_INITIALIZED){
            FSS->Active = HW_STOPPED;
            CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "Fine Sun Sensor stopped. Active: %d", FSS->Active);
        }
        else if(FSS->Active == HW_UNINITIALIZED){
            CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "Fine Sun Sensor has not been initialized. Cannot stop | Active: %d", FSS->Active);
        }
        else{
            CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "Fine Sun Sensor has not been started. Cannot stop | Active: %d", FSS->Active);
        }

        return Status;
    #endif // _ADCS_CHALLENGE

    // TODO: Status should return right status?
} 

int32_t ADCS_HW_FSS_Reset(ADCS_HW_Device_t *self) {
    ADCS_FSS_Class   *FSS = (ADCS_FSS_Class *) self->DeviceClassPtr;
    // ADCS_IMPL_FswPkt *fss_tlm = &ADCSImpl->FswTlmPkt;
    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
        if(FSS->Active > HW_RUNNING){
            FSS->Active = HW_STOPPED;
            for(int axis = 0; axis < ADCS_FSS_NUM_ANG; axis++){
                FSS->TlmPkt.FssTlmData.SunAng[axis]  = 0.5;
            }
            for(int axis = 0; axis < ADCS_FSS_NUM_VEC; axis++){
                FSS->TlmPkt.FssTlmData.SunVecS[axis] = 0;
                FSS->TlmPkt.FssTlmData.SunVecB[axis] = 0;
            }
            for(int axis = 0; axis < ADCS_FSS_NUM_VEC; axis++){
                FSS->TlmPkt.Cal.Calibration[axis] = 0;
            }
            FSS->TlmPkt.Cal.CalibrationType = FSS_SCALAR;
            FSS->Active = HW_RUNNING;
        }
        FSS->TlmPkt.DeviceTlm.DeviceResetCnt += 1;

        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "Fine Sun Sensor Reset. Active: %d | ResetCnt: %d", &FSS->Active, FSS->TlmPkt.DeviceTlm.DeviceResetCnt);
        // TODO: This should call a function that clears all its registers or something
        
        return ADCS_HW_FSS_Init(self);
    #endif // _FSS_CHALLENGE_
}

int32_t ADCS_HW_FSS_ResetStatus(ADCS_HW_Device_t *self) {
    // TODO - Add any FSS specific status
    ADCS_FSS_Class* FSS = (ADCS_FSS_Class *) self->DeviceClassPtr;
    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
        CFE_PSP_MemSet(&FSS->TlmPkt.DeviceTlm, 0, sizeof(ADCS_HW_DeviceTlmCommon_t));

        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "Fine Sun Sensor Reset Status. Active: %d |", FSS->Active);
        
        return ADCS_HW_EXEC_SUCCESS;
    #endif // _FSS_CHALLENGE_
}

int32_t ADCS_HW_FSS_Update(ADCS_HW_Device_t *self) {
    ADCS_FSS_Class* FSS = (ADCS_FSS_Class *) self->DeviceClassPtr;
    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
    // TODO: Pull values from 42 or some ish?
        FSS->TlmPkt.DeviceTlm.DeviceUpdateCnt += 1;

        return ADCS_HW_EXEC_SUCCESS;
    #endif // _FSS_CHALLENGE_

}

int32_t ADCS_HW_FSS_TLM_Update(ADCS_HW_Device_t *self) {
    int32_t Status = ADCS_HW_EXEC_SUCCESS;
    ADCS_FSS_Class* FSS = (ADCS_FSS_Class *) self->DeviceClassPtr;

    // Populate Telemetry packet with actual sensor data from last run

    #if _FSS_CHALLENGE_ == 0
        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_INFORMATION,
            "This module is deactivated");
        return ADCS_HW_EXEC_FAILURE;
    #else
        if(FSS->Active >= HW_RUNNING){
            FSS->TlmPkt.FssTlmData.Valid = FSS->data.Valid;
            
            for(int axis = 0; axis < ADCS_FSS_NUM_ANG; axis++){
                FSS->TlmPkt.FssTlmData.SunAng[axis]  = FSS->data.SunAng[axis];
                switch(FSS->Cal->CalibrationType){
                    case 0:
                        if (FSS->Cal->Calibration[axis] < 1.0){
                            FSS->TlmPkt.FssTlmData.SunAng[axis] *= FSS->Cal->Calibration[axis];
                        }
                    case 1:
                        if ((FSS->TlmPkt.FssTlmData.SunAng[axis] + FSS->Cal->Calibration[axis]) < 1.0){
                            FSS->TlmPkt.FssTlmData.SunAng[axis] += FSS->Cal->Calibration[axis];
                        }
                }
            }
            for(int axis = 0; axis < ADCS_FSS_NUM_VEC; axis++){
                FSS->TlmPkt.FssTlmData.SunVecS[axis] = FSS->data.SunVecS[axis];
                FSS->TlmPkt.FssTlmData.SunVecB[axis] = FSS->data.SunVecB[axis];
            }
            for(int axis = 0; axis < ADCS_FSS_NUM_VEC; axis++){
                FSS->TlmPkt.Cal.Calibration[axis] = FSS->Cal->Calibration[axis];
            }
            FSS->TlmPkt.Cal.CalibrationType = FSS->Cal->CalibrationType;
        }

        Status = ADCS_HW_MGR_UpdateCommonTlm(&FSS->TlmPkt.DeviceTlm, self);

        ADCS_HW_Device_t* Device = self;

        CFE_EVS_SendEvent(ADCS_HW_FSS_DEBUG_EID, CFE_EVS_DEBUG, "****  ADCS_HW_FSS_TLM_Update()");
        return Status;

    #endif // _FSS_CHALLENGE
}

