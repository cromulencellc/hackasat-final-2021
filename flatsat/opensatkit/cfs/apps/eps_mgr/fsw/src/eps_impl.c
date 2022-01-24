
#include "eps_impl.h"
#include "eps_logger.h"
#include "AcTypeConverter.h"

static EPS_IMPL_Class* EPSImpl = NULL;

void EPSAllocateAC(struct AcType *AC);
void EPSInitAC(struct AcType *AC);
int EPS_IMPL_ProcessSensorSb(void);
void EPS_IMPL_UpdateTlm(void);
void EPS_IMPL_SendSimStatusPkt(void);
int32_t EPS_IMPL_SetComponentState(uint8_t ComponentIdx, uint8_t ComponentState);
int32_t EPS_IMPL_SetMode(uint8_t Mode);
void EPS_IMPL_SetupComponentMapTbl(void);

/******************************************************************************
** Function: EPS_IMPL_Constructor
**
** Initialize a EPS_IMPL object.
**
** Notes:
**   1. This must be called prior to any other function.
**   2. Assumes IF42 has constructed osk_42_lib's AC42 shared data structure 
**
*/
void EPS_IMPL_Constructor(EPS_IMPL_Class*  EPSImplObj) {
    
    EPSImpl = EPSImplObj;

    /* If a class state variable can't default to zero then must be set after this */
    CFE_PSP_MemSet((void*)EPSImpl, 0, sizeof(EPS_IMPL_Class));
          
    // EPS_CONF_TBL_Constructor(&(EPSImpl->ConfTbl), EPS_IMPL_GetConfTblPtr, EPS_IMPL_LoadConfTbl, EPS_IMPL_LoadConfTblEntry);
    
    CFE_SB_CreatePipe(&EPSImpl->SensorPipe, EPS_SENSOR_PIPE_DEPTH, EPS_SENSOR_PIPE_NAME);
    CFE_SB_InitMsg(&EPSImpl->FswTlmPkt, EPS_MGR_FSW_TLM_HK_MID, EPS_IMPL_TLM_FSW_PKT_LEN, TRUE);
    
    CFE_SB_InitMsg((CFE_SB_Msg_t *) &EPSImpl->SimStatusPkt, EPS_SIM_STATUS_MID, EPS_SIM_STATUS_PKT_LEN, TRUE);

    // Set Startup Control Mode
    EPSImpl->CtrlMode = 0;
    EPSImpl->Init = true;

    // Allocate Space for Dynamic FSW Structures
    EPSAllocateAC(&EPSImpl->AC);
    EPSInitAC(&EPSImpl->AC);
    EPS_IMPL_SetupComponentMapTbl();
    EPSImpl->Debug = EPS_IMPL_DEBUG;
    EPS_CFG_Constructor(&EPSImpl->ConfTbl,
                        EPS_CFG_GetTblPtr,
                        EPS_CFG_LoadTbl,
                        EPS_CFG_LoadTblEntry);

    CFE_SB_Subscribe(EPS_SIM_SENSOR_MID, EPSImpl->SensorPipe);

} /* End EPS_IMPL_Constructor() */


/**********************************************************************/
/* This function copies needed parameters from the SC structure to    */
/* the AC structure.  This is a crude first pass.  It only allocates  */
/* memory for the structures, and counts on the data to be filled in  */
/* via messages.                                                      */
void EPSAllocateAC(struct AcType *AC)
{

   /* Clear Memory */
   memset(AC, 0, sizeof(struct AcType));

   /* EPS SIM ONLY */
   AC->Neps = NUM_EPS;
   if (AC->Neps > 0) {
      AC->EPS = (struct AcEPSType *) calloc(AC->Neps, sizeof(struct AcEPSType));
   }
   AC->Nsolarr = NUM_SOLAR_ARRAY;
   if (AC->Nsolarr > 0) {
      AC->SolArr = (struct AcSolarArrayType *) calloc(AC->Nsolarr, sizeof(struct AcSolarArrayType));
   }

}

void EPSInitAC(struct AcType *AC)
{
   AC->Init = 1;
   AC->ID = 0;
   /* Controllers */
   AC->DT = 0.2; // Delta time between steps
//    AC->EchoEnabled = TRUE;
}


int EPS_IMPL_ProcessSensorSb(void) {
    int32           CfeStatus;
    CFE_SB_Msg_t*   SbMsgPtr;
    EPS_SensorDataPkt_t * SensorDataPkt;
    CFE_SB_MsgId_t  MsgId;
    uint16          MsgLen;
    // uint16          ExecuteCycle = 0;
    // boolean         ExecuteLoop  = FALSE;

    // CfeStatus = CFE_SB_RcvMsg(&SbMsgPtr, EPSImpl->SensorPipe, 50);

    // Wait until we get updated sensor data before continueing forward
    CfeStatus = CFE_SB_RcvMsg(&SbMsgPtr, EPSImpl->SensorPipe, CFE_SB_PEND_FOREVER);
    if (CfeStatus == CFE_SUCCESS ) {
        CFE_EVS_SendEvent(EPS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "EPS_IMPL_ProcessSensorSb() Recieved SensorDataPkt SB Msg");
        EPS_LOG_DEBUG("EPS_IMPL_ProcessSensorSb() Recieved SensorDataPkt SB Msg");
        MsgId = CFE_SB_GetMsgId(SbMsgPtr); 
        if (MsgId == EPS_SIM_SENSOR_MID) {
            MsgLen = CFE_SB_GetUserDataLength(SbMsgPtr);
            SensorDataPkt = (EPS_SensorDataPkt_t *) SbMsgPtr;
            if( MsgLen > 0) {
                StringToAcType(SensorDataPkt->Data, MsgLen, &EPSImpl->AC);
                EPS_LOG_DEBUG("EPS Sensor Msg:\n%s",SensorDataPkt->Data);
            } else {
                CfeStatus = -1;
            }
        }
    } else {
        CFE_EVS_SendEvent(EPS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "EPS_IMPL_ProcessSensorSb() Timeout on SensorDataPkt SB Msg");
    }
    return CfeStatus;
}

int32 EPS_IMPL_Run(void) {
    if(EPSImpl->Debug) {
        EPS_LOG_INFO("EPS_IMPL_Run() Started");
    }
    int32 Status;
    CFE_TIME_SysTime_t time_val = CFE_TIME_GetTime();
    uint32_t usecs = CFE_TIME_Sub2MicroSecs(time_val.Subseconds);
    double time_secs = (double) time_val.Seconds + ((double)usecs / 1E6); // convert to seconds
    EPSImpl->FswTlmPkt.CtrlCfsExeTime = time_secs;
    CFE_EVS_SendEvent(EPS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "EPS_IMPL_Run() (ExeCnt: %d, Time: %f)", (int) EPSImpl->CtrlExeCnt, time_secs);
    Status = EPS_IMPL_ProcessSensorSb();
    if (Status == CFE_SUCCESS) {
        EPS_LOG_DEBUG("RUN EPS FSW HERE");
        EPSImpl->CtrlExeCnt++;
        
    } else {
        CFE_EVS_SendEvent(EPS_IMPL_ERROR_EID, CFE_EVS_DEBUG, "EPS_IMPL_Run() EPS Sensor Data Update Failure");
    }
    
    if(EPSImpl->Debug) {
        EPS_LOG_INFO("EPS_IMPL_Run() Completed.");
    }
    return Status;

    // EPS_LOG_INFO("Finished ADCS RunLoop");
}

void EPS_IMPL_Update(void) {
    if (EPSImpl->Init) {
        EPS_IMPL_SetMode(EPSImpl->ConfTbl.Tbl.StartupMode);
        CFE_EVS_SendEvent(EPS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "EPS Started in Mode %d", EPSImpl->CtrlMode);
        EPSImpl->Init = false;
    }
    EPS_IMPL_SendSimStatusPkt();
    EPS_IMPL_UpdateTlm();
}

void EPS_IMPL_UpdateTlm(void) {
    EPS_IMPL_FswPkt *TLM = &EPSImpl->FswTlmPkt; 
    CFE_EVS_SendEvent(EPS_IMPL_DEBUG_EID, CFE_EVS_DEBUG, "EPS_IMPL_UpdateTlm() Update TLM Struct\n");
    // CFE_PSP_MemCpy(&TLM->EpsTlm,  EPSImpl->AC.EPS, sizeof(struct AcEPSType));
    TLM->CtrlMode = EPSImpl->CtrlMode;
    TLM->CtrlExeCnt = EPSImpl->CtrlExeCnt;

    // Swtich States
    TLM->SwitchState[EPS_COMM_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->COM_Switch;
    TLM->SwitchState[EPS_ADCS_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->ADCS_Switch;
    TLM->SwitchState[EPS_WHEEL_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->Wheel_Switch;
    TLM->SwitchState[EPS_IMU_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->IMU_Switch;
    TLM->SwitchState[EPS_STARTRACKER_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->StarTracker_Switch;
    TLM->SwitchState[EPS_MTR_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->MTR_Switch;
    TLM->SwitchState[EPS_CSS_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->CSS_Switch;
    TLM->SwitchState[EPS_FSS_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->FSS_Switch;
    TLM->SwitchState[EPS_PAYLOAD_SWITCH_IDX] = (uint8_t) EPSImpl->AC.EPS->PAYLOAD_Switch;

    // Currents
    TLM->Wheel_I = (float) EPSImpl->AC.EPS->Wheel_I;
    TLM->IMU_I = (float) EPSImpl->AC.EPS->IMU_I;
    TLM->StarTracker_I = (float) EPSImpl->AC.EPS->StarTracker_I;
    TLM->MTR_I = (float) EPSImpl->AC.EPS->MTR_I;
    TLM->CSS_I = (float) EPSImpl->AC.EPS->CSS_I;
    TLM->FSS_I = (float) EPSImpl->AC.EPS->FSS_I;
    TLM->ADCS_I = (float) EPSImpl->AC.EPS->ADCS_I;
    TLM->CDH_I = (float) EPSImpl->AC.EPS->CDH_I;
    TLM->COM_I = (float) EPSImpl->AC.EPS->COM_I;
    TLM->PAYLOAD_I = (float) EPSImpl->AC.EPS->PAYLOAD_I;
   
    // Voltages
    TLM->Wheel_V = (float) EPSImpl->AC.EPS->Wheel_V;
    TLM->MTR_V = (float) EPSImpl->AC.EPS->MTR_V;
    TLM->IMU_V = (float) EPSImpl->AC.EPS->IMU_V;
    TLM->StarTracker_V = (float) EPSImpl->AC.EPS->StarTracker_V;
    TLM->MTR_V = (float) EPSImpl->AC.EPS->MTR_V;
    TLM->CSS_V = (float) EPSImpl->AC.EPS->CSS_V;
    TLM->FSS_V = (float) EPSImpl->AC.EPS->FSS_V;
    TLM->ADCS_V = (float) EPSImpl->AC.EPS->ADCS_V;
    TLM->CDH_V = (float) EPSImpl->AC.EPS->CDH_V;
    TLM->COM_V = (float) EPSImpl->AC.EPS->COM_V;
    TLM->PAYLOAD_V = (float) EPSImpl->AC.EPS->PAYLOAD_V;
    // Battery
    TLM->BATTERY_SOC = (float) EPSImpl->AC.EPS->BATTERY_SOC;
    TLM->BATTERY_V = (float) EPSImpl->AC.EPS->BATTERY_V;
    TLM->BATTERY_I = (float) EPSImpl->AC.EPS->BATTERY_I;
    // Solar Array
    for (int i=0;i<EPSImpl->AC.Nsolarr;i++) {
        TLM->SOLAR_VALID[i] = EPSImpl->AC.SolArr[i].Valid;
        TLM->SOLAR_P[i] = EPSImpl->AC.SolArr[i].PowerOut;
        TLM->SOLAR_I[i] = EPSImpl->AC.SolArr[i].CurrentOut;
        TLM->SOLAR_V[i] = EPSImpl->AC.SolArr[i].Voltage;
    }
    // Set Ctrl J2000 Time
    TLM->CtrlTimeJ2000 = EPSImpl->AC.Time;

    // Timestamp and Send Telemetry
    CFE_SB_TimeStampMsg((CFE_SB_Msg_t *) TLM);
    int32_t Status = CFE_SB_SendMsg((CFE_SB_Msg_t *) TLM);
    if (Status != CFE_SUCCESS) {
        EPS_LOG_ERROR("EPS FSW TLM send failure");
    }
}

void EPS_IMPL_SendSimStatusPkt(void) {
    int32 Status;
    CFE_PSP_MemSet(&EPSImpl->SimStatusPkt.Data, 0, sizeof(EPSImpl->SimStatusPkt.Data));
    size_t actuator_pkt_size = AcTypeToString(&EPSImpl->AC, (char * ) EPSImpl->SimStatusPkt.Data);
    CFE_SB_Msg_t*  MsgPtr = (CFE_SB_Msg_t*)&EPSImpl->SimStatusPkt;
    EPS_LOG_DEBUG("EPS Status Pkt:\n%s", EPSImpl->SimStatusPkt.Data);
    if (MsgPtr != NULL && actuator_pkt_size <= SIM_PKT_SIZE) {
        CFE_SB_TimeStampMsg(MsgPtr);
        Status = CFE_SB_SendMsg(MsgPtr);
        if (Status != CFE_SUCCESS) {
            EPS_LOG_ERROR("EPS Sim Status Msg send failure");
        }
    }
}

/**
 *  Commands
 */
boolean EPS_IMPL_SetModeCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    EPS_IMPL_SetModeCmdPkt* CmdPkt = (EPS_IMPL_SetModeCmdPkt*) MsgPtr;
    int32_t Status = -1;
    if (CmdPkt->EpsMode < EPS_MODE_CNT) {
        CFE_EVS_SendEvent(EPS_IMPL_CMD_EID, CFE_EVS_INFORMATION, "EPS_IMPL_SetModeCmd() Command Recieved for Mode %d", CmdPkt->EpsMode);
        Status = EPS_IMPL_SetMode(CmdPkt->EpsMode);
    } else {
        CFE_EVS_SendEvent(EPS_IMPL_CMD_ERROR_EID, CFE_EVS_INFORMATION, "EPS_IMPL_SetModeCmd() Invalid Command Recieved for Mode %d", CmdPkt->EpsMode);

    }
    if (Status == CFE_SUCCESS) {
        return TRUE;
    } else {
        return FALSE;
    }
}

boolean EPS_IMPL_SetComponentStateCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr) {
    EPS_IMPL_SetComponentStateCmdPkt* CmdPkt = (EPS_IMPL_SetComponentStateCmdPkt*) MsgPtr;
    int32_t Status = -1;

    if (CmdPkt->ComponentIdx < EPS_SWITCH_CNT) {
        CFE_EVS_SendEvent(EPS_IMPL_CMD_EID, CFE_EVS_INFORMATION, "EPS_IMPL_SetComponentStateCmd() Command Recieved for Switch Idx: %d state %d", CmdPkt->ComponentIdx, CmdPkt->ComponentState);
        Status = EPS_IMPL_SetComponentState(CmdPkt->ComponentIdx, CmdPkt->ComponentState);
    } else {
        CFE_EVS_SendEvent(EPS_IMPL_CMD_ERROR_EID, CFE_EVS_INFORMATION, "EPS_IMPL_SetComponentStateCmd() Invalid Command Recieved for Switch Idx: %d state %d", CmdPkt->ComponentIdx, CmdPkt->ComponentState);

    }
    if (Status == CFE_SUCCESS) {
        return TRUE;
    } else {
        return FALSE;
    }
}


/**
 *  Utility Functions
 */
int32_t EPS_IMPL_SetComponentState(uint8_t ComponentIdx, uint8_t ComponentState) {
    int32_t Status = -1;
    if (ComponentIdx <= (EPS_SWITCH_CNT-1)) {
        EPS_IMPL_ComponentMap_t* componentMapPtr = &EPSImpl->ComponentMapTbl[ComponentIdx];
        uint8_t modeAllowTable[EPS_SWITCH_CNT];
        uint8_t componentModeAllowableState = EPSImpl->ConfTbl.Tbl.ModeTbl[EPSImpl->CtrlMode].ModeSwitchTbl[ComponentIdx];
        bool stateAllowed = false;
        if (ComponentState == 1 && componentModeAllowableState == 1) {
            stateAllowed = true;
        } else if (ComponentState == 0) {
            stateAllowed = true;
        }
        
        if (stateAllowed) {
            switch (ComponentState)
            {
            case 0:
                CFE_EVS_SendEvent(EPS_IMPL_FSW_EID, CFE_EVS_INFORMATION, "EPS_IMPL_SetComponentState() ComponentIdx %d to OFF", ComponentIdx);
                if (componentMapPtr->ComponentIdx == ComponentIdx) {
                    *componentMapPtr->SwitchStatePtr = (long) ComponentState;
                }
                Status = CFE_SUCCESS;
                break;
            case 1:
                CFE_EVS_SendEvent(EPS_IMPL_FSW_EID, CFE_EVS_INFORMATION, "EPS_IMPL_SetComponentState() ComponentIdx %d to ON", ComponentIdx);
                if (componentMapPtr->ComponentIdx == ComponentIdx) {
                    *componentMapPtr->SwitchStatePtr = (long) ComponentState;
                }
                Status = CFE_SUCCESS;
                break;
            default:
                CFE_EVS_SendEvent(EPS_IMPL_FSW_EID, CFE_EVS_ERROR, "EPS_IMPL_SetComponentState() Unknown Component Idx %d", ComponentIdx);
                break;
            }
        } else {
            CFE_EVS_SendEvent(EPS_IMPL_FSW_EID, CFE_EVS_ERROR, "EPS_IMPL_SetComponentState() State %d not Allowed for Component Idx %d in Mode %d. Allowed State is %d", 
                ComponentState, ComponentIdx, EPSImpl->CtrlMode, componentModeAllowableState);
        }

    }
    return Status;
}

int32_t EPS_IMPL_SetMode(uint8_t Mode) {
    int32_t Status = -1;
    if (Mode < EPS_MODE_CNT) {
        EPS_CFG_ModeEntry* modeEntryPtr = &EPSImpl->ConfTbl.Tbl.ModeTbl[Mode];
        if (modeEntryPtr->Enabled) {
            EPSImpl->CtrlMode = Mode;
            CFE_EVS_SendEvent(EPS_IMPL_FSW_EID, CFE_EVS_INFORMATION, "EPS_IMPL_SetMode() Set EPS Mode to mode %s", modeEntryPtr->ModeName);
            for (int i=0; i<EPS_SWITCH_CNT; i++) {
                Status = EPS_IMPL_SetComponentState(i, modeEntryPtr->ModeSwitchTbl[i]);
            }
        } else {
            CFE_EVS_SendEvent(EPS_IMPL_FSW_EID, CFE_EVS_ERROR, "EPS_IMPL_SetMode() Set EPS Mode to mode %s not allowed, mode is set to disabled state %d", modeEntryPtr->ModeName, modeEntryPtr->Enabled);

        }
    }
    return Status;
}


void EPS_IMPL_SetupComponentMapTbl(void) {
    EPS_IMPL_ComponentMap_t componentTbl[EPS_SWITCH_CNT] = {
        {
            .ComponentIdx = EPS_COMM_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->COM_Switch
        },
        {
            .ComponentIdx = EPS_ADCS_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->ADCS_Switch
        },
                {
            .ComponentIdx = EPS_WHEEL_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->Wheel_Switch
        },
                {
            .ComponentIdx = EPS_IMU_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->IMU_Switch
        },
                {
            .ComponentIdx = EPS_STARTRACKER_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->StarTracker_Switch
        },
                {
            .ComponentIdx = EPS_MTR_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->MTR_Switch
        },
                {
            .ComponentIdx = EPS_CSS_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->CSS_Switch
        },
                {
            .ComponentIdx = EPS_FSS_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->FSS_Switch
        },
                {
            .ComponentIdx = EPS_PAYLOAD_SWITCH_IDX,
            .SwitchStatePtr = &EPSImpl->AC.EPS->PAYLOAD_Switch
        },
    };
    CFE_PSP_MemCpy(EPSImpl->ComponentMapTbl, componentTbl, sizeof(componentTbl));
}