#ifndef _adcs_impl_h_
#define _adcs_impl_h_

/*
** Includes
*/
#include "app_cfg.h"
#include "adcs_conf_tbl.h"
#include "AcTypes.h"
#include "adcs_hw.h"
#include "imu.h"
#include "mtr.h"
#include "rw.h"
#include "xadc.h"
// _ADCS_CHALLENGE_
#include "fss.h"

/*
** Event Message IDs
*/

#define ADCS_IMPL_SET_CTRL_MODE_EID        (ADCS_IMPL_BASE_EID +  0)
#define ADCS_IMPL_INVLD_CTRL_MODE_EID      (ADCS_IMPL_BASE_EID +  1) // TODO - Implement in handler
#define ADCS_IMPL_SET_OVR_EID              (ADCS_IMPL_BASE_EID +  2) // TODO - Implement in handler
#define ADCS_IMPL_INVLD_OVR_STATE_EID      (ADCS_IMPL_BASE_EID +  3) // TODO - Implement in handler
#define ADCS_IMPL_INVLD_OVR_ID_EID         (ADCS_IMPL_BASE_EID +  4) // TODO - Implement in handler
#define ADCS_IMPL_SET_TARGET_WHL_MOM_EID   (ADCS_IMPL_BASE_EID +  5) // TODO - Implement in handler
#define ADCS_IMPL_INVLD_TARGET_WHL_MOM_EID (ADCS_IMPL_BASE_EID +  6) // TODO - Implement in handler
#define ADCS_IMPL_SET_TARGET_WHL_SPD_EID   (ADCS_IMPL_BASE_EID +  7)
#define ADCS_IMPL_INVLD_TARGET_WHL_SPD_EID (ADCS_IMPL_BASE_EID +  8)  
#define ADCS_IMPL_DEBUG_CMD_EID            (ADCS_IMPL_BASE_EID +  9)
#define ADCS_IMPL_CMD_ERR_EID              (ADCS_IMPL_BASE_EID +  10)
#define ADCS_IMPL_ERROR_EID                (ADCS_IMPL_BASE_EID +  11)
#define ADCS_IMPL_DEBUG_EID                (ADCS_IMPL_BASE_EID + 12)
#define ADCS_IMPL_DEBUG_FSW_EID            (ADCS_IMPL_BASE_EID + 13)
#define ADCS_IMPL_ERROR_FSW_EID            (ADCS_IMPL_BASE_EID + 14)
/******************************************************************************
** Control Packets
*/

/******************************************************************************
** Command Packets
*/

typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t Debug;
    float   Vmax;
    float   Amax;
    float   Wc;
    float   Ixx;
    float   Iyy;
    float   Izz;

}  OS_PACK ADCS_IMPL_SetCtrlParamCmdPkt;
#define ADCS_IMPL_SET_CTRL_PARAM_CMD_DATA_LEN  (sizeof(ADCS_IMPL_SetCtrlParamCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    int16   NewMode;

}  OS_PACK ADCS_IMPL_SetCtrlModeCmdPkt;
#define ADCS_IMPL_SET_CTRL_MODE_CMD_DATA_LEN  (sizeof(ADCS_IMPL_SetCtrlModeCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8   FrameTgt;
    float   Qtr[4];

}  OS_PACK ADCS_IMPL_SetCtrlTgtCmdPkt;
#define ADCS_IMPL_SET_CTRL_TGT_CMD_DATA_LEN  (sizeof(ADCS_IMPL_SetCtrlTgtCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    float   SunYawTgt; /**< @brief [float] SunYaw control target angle in radians (0 - 2pi) */
    float   SunYawDeadband; /**< @brief [float] SunYaw control deadband angle in radians (0 - 2pi) */
    float   SunYawRateDeadband; /**< @brief [float] SunYaw rate control deadband radians/s */

}  OS_PACK ADCS_IMPL_SetSunYawCtrlCmdPkt;
#define ADCS_IMPL_SET_SUN_YAW_CTRL_CMD_DATA_LEN  (sizeof(ADCS_IMPL_SetSunYawCtrlCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

    uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8    Id;
    uint8    State;

}  OS_PACK ADCS_IMPL_SetOvrCmdPkt;
#define ADCS_IMPL_SET_OVR_CMD_DATA_LEN  (sizeof(ADCS_IMPL_SetOvrCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    float   Whl[ADCS_WHEEL_COUNT];

}  OS_PACK ADCS_IMPL_SetTargetWhlMomCmdPkt;
#define ADCS_IMPL_SET_TARGET_WHL_MOM_CMD_DATA_LEN  (sizeof(ADCS_IMPL_SetTargetWhlMomCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8_t    CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16_t   NewState;                        /* 0=Disable, 1=Enable */

}  OS_PACK ADCS_IMPL_ConfigDbgCmdPkt;
#define ADCS_IMPL_CONFIG_DBG_CMD_DATA_LEN  (sizeof(ADCS_IMPL_ConfigDbgCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

    uint8_t     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t     Component; // Must map to uint8_t component index
    uint8_t     HwCmd; // Must map to ADCS_HW_CMD_t

}  OS_PACK ADCS_IMPL_HardwareCmdPkt;
#define ADCS_IMPL_HARDWARE_CMD_DATA_LEN  (sizeof(ADCS_IMPL_HardwareCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

    uint8_t     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t     Enable; // 0 = Disable, 1 = Enable
}  OS_PACK ADCS_IMPL_HardwareTlmEnableCmdPkt;
#define ADCS_IMPL_HARDWARE_TLM_ENABLE_CMD_DATA_LEN  (sizeof(ADCS_IMPL_HardwareTlmEnableCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

    uint8_t     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t     Id; // Must map to torque rod indexes
    uint8_t     State; // Must map to MtrState

}  OS_PACK ADCS_IMPL_TorqueRodStateCmdPkt;
#define ADCS_IMPL_TORQUE_ROD_STATE_CMD_DATA_LEN  (sizeof(ADCS_IMPL_TorqueRodStateCmdPkt) - CFE_SB_CMD_HDR_SIZE)


typedef struct {
    uint8_t                     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t                     WheelId;
    int16_t                     WheelSpeed; //RPM
} OS_PACK ADCS_IMPL_WheelSpeedCmdPkt;
#define ADCS_IMPL_WHEEL_SPEED_CMD_DATA_LEN  (sizeof(ADCS_IMPL_WheelSpeedCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {
    uint8_t                     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t                     WheelId;
    uint8_t                     frameId; /**< @brief [uint8_t] Telecommand ID */
    uint8_t                     dataLen;  /**< @brief [uint8_t] Number of data bytes */
    uint8_t                     data[CUBEWHEEL_TC_MAX_LEN];  /**< @brief byte array of data */
} OS_PACK ADCS_IMPL_WheelRawCmdPkt;
#define ADCS_IMPL_WHEEL_RAW_CMD_DATA_LEN  (sizeof(ADCS_IMPL_WheelRawCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {
    uint8_t                     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t                     CalibrationType;
    uint8_t                     CalibrationSubtype;
} OS_PACK ADCS_IMPL_CalibrationCmdPkt;
#define ADCS_IMPL_CALIBRATION_CMD_DATA_LEN  (sizeof(ADCS_IMPL_CalibrationCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {
    uint8_t                     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t                     IsWrite;
    uint32_t                    Addr;
    uint32_t                    Data;
} OS_PACK ADCS_IMPL_StarTrackerRegisterCmdPkt;
#define ADCS_IMPL_STAR_TRACKER_REG_CMD_DATA_LEN  (sizeof(ADCS_IMPL_StarTrackerRegisterCmdPkt) - CFE_SB_CMD_HDR_SIZE)

// _ADCS_CHALLENGE_
typedef struct {
    uint8_t                     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t                     CalibrationType;
    char                        CalibrationMatrix[64];
} OS_PACK ADCS_IMPL_FineSunSensorCalibrationCmdPkt;
#define ADCS_IMPL_FINE_SUN_SENSOR_CALIBRATION_CMD_DATA_LEN (sizeof(ADCS_IMPL_FineSunSensorCalibrationCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {
    uint8_t                     CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t                     update;
} OS_PACK ADCS_IMPL_UpdateFlagCmdPkt;
#define ADCS_IMPL_UPDATE_FLAG_CMD_DATA_LEN (sizeof(ADCS_IMPL_UpdateFlagCmdPkt) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

    uint8  Header[CFE_SB_TLM_HDR_SIZE];
    
    /* Flag Tlm */
    uint8_t CtrlMode;
    uint8_t AttitudeSource;
    uint16_t AttitudeSourceStepCnt;
    uint8_t CtrlLawResult;
    uint8_t MtbMode;
    #if _FSS_CHALLENGE_ == 0
        uint8_t HwTlmStatus[ADCS_HW_DEVICE_CNT_MAX+1];
    #else
        uint8_t HwTlmStatus[ADCS_HW_DEVICE_CNT_MAX];
    #endif // _FSS_CHALLENGE_
    uint8_t Flags[11];
    uint8_t ErrorFlags[9];

    /* State Tlm */
    float  qbn[4];
    float  qbr[4];
    float  qbl[4];
    float  qtr[4];
    float  wbn[3];
    float  werr[3];
    float  wln[3];
    float  Hvb[3];
    float  svb[3];
    float  acc[3]; // Acceleration in body frame
    float  bvb[3]; // Magnetic field body frame

    float  Ww[ADCS_WHEEL_COUNT]; // Current wheel speeds
    float  Tcmd[ADCS_WHEEL_COUNT];      /* Torque Command   */
    float  WwCmd[ADCS_WHEEL_COUNT];     /* Wheel Speed Command */
    float  Hw[ADCS_WHEEL_COUNT];        /* Wheel Momemtum */
    float  Mcmd[ADCS_MTR_NUM_CHANNELS]; /* Magentic Command */
    float  AnalogMonitor[ADCS_NUM_ANALOG_MONITOR];
    float  CssIllum[NUM_CSS_SENSORS];
    // float  CssIllumBackground[NUM_CSS_SENSORS];

    // 20 floats == 80 bytes
    // ADCS_CHALLENGE
    float FssSunAng[NUM_FSS_SENSORS][ADCS_FSS_NUM_ANG];
    // 8 floats == 24 bytes

    uint32_t WheelSpeedCmdCnt; // Counter for wheel speed commands
    #ifdef _FLATSAT_HARDWARE_FSW
    float  SunYaw;
    float  SunYawTgt;
    float  SunYawDeadband;
    float  SunYawRateDeadband;
    #endif
    // 5 floats == 20 bytes

    /* Control Loop Tlm */
    float  Kunl;
    float  Vmax;
    float  Amax;
    float  Wc;
    float  Ixx;
    float  Iyy;
    float  Izz;
    // 7 floats == 28 bytes
    double CtrlTimeJ2000;
    double CtrlCfsExeTime; // CFE Seconds

} OS_PACK ADCS_IMPL_FswPkt;
#define ADCS_IMPL_TLM_FSW_PKT_LEN sizeof (ADCS_IMPL_FswPkt)


#define SIM_PKT_SIZE 2500

typedef struct {
    uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
    // uint16_t DataLen;
    char Data[SIM_PKT_SIZE];
} OS_PACK ADCS_SimActuatorPkt_t;
#define ADCS_SIM_ACTUATOR_PKT_LEN sizeof (ADCS_SimActuatorPkt_t)


typedef struct {
   uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    uint16_t DataLen;
   char    Data[SIM_PKT_SIZE];
} OS_PACK ADCS_SensorDataPkt_t;
#define ADCS_SIM_SIM_PKT_LEN sizeof (ADCS_SensorDataPkt_t)

/******************************************************************************
** F42 Adapter Class
*/

typedef struct {

    /*
    ** ADCS_IMPL Data 
    */
    uint16_t   CtrlMode;
    uint16_t   AttitudeSource;
    uint32   CtrlExeCnt;
    boolean  Init;
    CFE_SB_PipeId_t SensorPipe;

    double TimeStart;
    /*
    ** Telemetry
    */
    ADCS_IMPL_FswPkt        FswTlmPkt;
    
    #ifdef _FLATSAT_SIM_FSW_
    ADCS_SimActuatorPkt_t   SimActuatorPkt;
    #endif

    /*
    ** Contained Objects
    */
    ADCS_CONF_TBL_Class     ConfTbl;
    ADCS_HW_MGR_Class       HW;
    ADCS_IMU_Class          IMU;
    ADCS_MTR_Class          MTR;
    ADCS_RW_Class           RW;
    ADCS_XADC_Class         XADC;

    // _ADCS_CHALLENGE_
    ADCS_FSS_Class          FSS;

    struct AcType           AC;

} ADCS_IMPL_Class;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: ADCS_IMPL_Constructor
**
** Initialize a ADCS_Impl object.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
void ADCS_IMPL_Constructor(ADCS_IMPL_Class*  ADCSImplObj);


/******************************************************************************
** Function: ADCS_IMPL_GetConfTblPtr
**
** Function to return point to control table.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
const ADCS_CONF_TBL_Struct* ADCS_IMPL_GetConfTblPtr();

/******************************************************************************
** Function: ADCS_IMPL_LoadConfTbl
**
** Fucntion to load full control table.
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
boolean ADCS_IMPL_LoadConfTbl(ADCS_CONF_TBL_Struct* NewTbl);


/******************************************************************************
** Function: ADCS_IMPL_LoadConfTblEntry
**
** Function to load Control Table Entry
**
** Notes:
**   1. This must be called prior to any other function.
**
*/
boolean ADCS_IMPL_LoadConfTblEntry(uint16 ObjId, void* ObjData);

boolean ADCS_IMPL_CompleteStartup(void);

boolean ADCS_IMPL_HardwareCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_HardwareTlmEnable(bool enable, uint8_t component);

boolean ADCS_IMPL_TorqueRodStateCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_SetCtrlMode(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_WheelSpeedCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_WheelRawCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_CalibrationCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_SunYawCtrlCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_SetCtrlParamsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_StarTrackerRegisterCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_FSSCalibrationCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_SetCtrlTarget(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

boolean ADCS_IMPL_UpdateFlagCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

void ADCS_IMPL_Run(void);


#endif /* _adcs_impl_h_ */