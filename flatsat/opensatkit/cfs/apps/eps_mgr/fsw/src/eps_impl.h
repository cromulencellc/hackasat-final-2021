#ifndef _eps_impl_h_
#define _eps_impl_h_

#include "app_cfg.h"
#include "AcTypes.h"
#include "eps_cfg_tbl.h"


/*
** Event Message IDs
*/

#define EPS_IMPL_ERROR_EID                  (EPS_IMPL_BASE_EID + 0)
#define EPS_IMPL_DEBUG_EID                  (EPS_IMPL_BASE_EID + 1)
#define EPS_IMPL_FSW_EID                    (EPS_IMPL_BASE_EID + 2)
#define EPS_IMPL_CMD_EID                    (EPS_IMPL_BASE_EID + 3)
#define EPS_IMPL_CMD_ERROR_EID              (EPS_IMPL_BASE_EID + 4)

#define SIM_PKT_SIZE 2500

/******************************************************************************
** Enumerations
*/
typedef enum {
    EPS_MODE_NONE=0,
    EPS_MODE_SEPERATION=1,
    EPS_MODE_SAFE=2,
    EPS_MODE_STANDBY=3,
    EPS_MODE_PAYLOAD_ON=4
} EpsMode;

typedef enum {
    EPS_COMM_SWITCH_IDX=0,
    EPS_ADCS_SWITCH_IDX=1,
    EPS_WHEEL_SWITCH_IDX=2,
    EPS_IMU_SWITCH_IDX=3,
    EPS_STARTRACKER_SWITCH_IDX=4,
    EPS_MTR_SWITCH_IDX=5,
    EPS_CSS_SWITCH_IDX=6,
    EPS_FSS_SWITCH_IDX=7,
    EPS_PAYLOAD_SWITCH_IDX=8
} EpsSwitchEnum;


/******************************************************************************
** Command Packets
*/

typedef struct {

    uint8_t   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t   EpsMode;

}  OS_PACK EPS_IMPL_SetModeCmdPkt;
#define EPS_IMPL_SET_MODE_CMD_PKT_LEN  (sizeof(EPS_IMPL_SetModeCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct {

    uint8_t   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t   ComponentIdx;
    uint8_t   ComponentState;

}  OS_PACK EPS_IMPL_SetComponentStateCmdPkt;
#define EPS_IMPL_SET_COMPONENT_STATE_CMD_PKT_LEN  (sizeof(EPS_IMPL_SetComponentStateCmdPkt) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {
    uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
    // uint16_t DataLen;
    char Data[SIM_PKT_SIZE];
} OS_PACK EPS_SimFeedbackPkt_t;
#define EPS_SIM_FEEDBACK_PKT_LEN sizeof (EPS_SimFeedbackPkt_t)

typedef struct {
   uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    uint16_t DataLen;
   char    Data[SIM_PKT_SIZE];
} OS_PACK EPS_SensorDataPkt_t;
#define EPS_SIM_STATUS_PKT_LEN sizeof (EPS_SensorDataPkt_t)


typedef struct {

    uint8  Header[CFE_SB_TLM_HDR_SIZE];
    
    /* Flag Tlm */
    uint8_t CtrlMode;
    uint16_t ErrorFlags;
    uint32_t CtrlExeCnt;

    uint8_t SwitchState[EPS_SWITCH_CNT];

    // Wheel
    float Wheel_I;               /* [[A]] [~>~] */
    float Wheel_V;               /* [[V]] [~>~] */
    // IMU
    float IMU_I;                 /* [[A]] [~>~] */
    float IMU_V;                 /* [[V]] [~>~] */
    // ST
    float StarTracker_I;         /* [[A]] [~>~] */
    float StarTracker_V;         /* [[V]] [~>~] */
    // MTR
    float MTR_I;                 /* [[A]] [~>~] */
    float MTR_V;                 /* [[V]] [~>~] */
    // CSS
    float CSS_I;                 /* [[A]] [~>~] */
    float CSS_V;                 /* [[V]] [~>~] */
    // FSS
    float FSS_I;                 /* [[A]] [~>~] */
    float FSS_V;                 /* [[V]] [~>~] */
    // ADCS
    float ADCS_I;                /* [[m] [~>~] */
    float ADCS_V;                /* [[V]] [~>~] */
    // CDH
    float CDH_I;                 /* [[A]] [~>~] */
    float CDH_V;                 /* [[V]] [~>~] */
    // TTC COMM
    float COM_I;                 /* [[A]] [~>~] */
    float COM_V;                 /* [[V]] [~>~] */
    // Payload COMM
    float PAYLOAD_I;             /* [[A]] [~>~] */
    float PAYLOAD_V;             /* [[V]] [~>~] */
    // Battery
    float BATTERY_I;            /* [[A]] [~>~] */
    float BATTERY_V;            /* [[V]] [~>~] */
    float BATTERY_SOC;          /* [[Unitless]] [~>~]*/
    // Solar Array
    long SOLAR_VALID[NUM_SOLAR_ARRAY];           /*Sun on array*/
    float SOLAR_P[NUM_SOLAR_ARRAY];              /* [[m]] [~>~] */
    float SOLAR_I[NUM_SOLAR_ARRAY];              /* [[A]] [~>~] */
    float SOLAR_V[NUM_SOLAR_ARRAY];              /* [[V]] [~>~] */
    // Control Execution Time
    double CtrlTimeJ2000;
    double CtrlCfsExeTime; // CFE Seconds


} OS_PACK EPS_IMPL_FswPkt;
#define EPS_IMPL_TLM_FSW_PKT_LEN sizeof (EPS_IMPL_FswPkt)

/******************************************************************************
** Type Defintions
*/

typedef struct {
    uint32_t    ComponentIdx;
    long*   SwitchStatePtr;
} EPS_IMPL_ComponentMap_t;


typedef struct {

    /*
    ** EPS_Impl Data 
    */
    uint16   CtrlMode;
    uint32   CtrlExeCnt;
    boolean  Init;
    boolean  Debug;
    CFE_SB_PipeId_t SensorPipe;

    double TimeStart;
    /*
    ** Telemetry
    */
    EPS_IMPL_FswPkt        FswTlmPkt;
    

    EPS_SimFeedbackPkt_t   SimStatusPkt;
    

    /*
    ** Contained Objects
    */
    EPS_CFG_Class           ConfTbl;
    EPS_IMPL_ComponentMap_t ComponentMapTbl[EPS_SWITCH_CNT];
    struct AcType           AC;

} EPS_IMPL_Class;

#endif

/**
 * Exported Functions
 */

int32 EPS_IMPL_Run(void);
void EPS_IMPL_Update(void);
void EPS_IMPL_Constructor(EPS_IMPL_Class*  EPSImplObj);

boolean EPS_IMPL_SetModeCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);
boolean EPS_IMPL_SetComponentStateCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);