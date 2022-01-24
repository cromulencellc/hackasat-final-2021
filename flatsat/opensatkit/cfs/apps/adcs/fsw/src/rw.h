#ifndef _wheel_h_
#define _wheel_h_

#include "adcs_io_lib.h"
#include "adcs_hw.h"
#include "adcs_conf_tbl.h"
#include "adcs_hw_cfg.h"
#include "tqueue.h"

#define ADCS_RW_CHILD_SEM_NAME     "RW_CHILD_SEM"
#define ADCS_RW_CHILD_NAME         "RW_CHILD"
#define ADCS_RW_STACK_SIZE         8096
#define ADCS_RW_CHILD_PRIORITY     40
#define ADCS_RW_SEM_INVALID        0xFFFFFFFF

#define ADCS_RW_CMDS_PER_LOOP        3
#define ADCS_RW_TLM_CMD_BUFF_COUNT   10
#define ADCS_RW_TC_CMD_BUFF_COUNT    10
/************************/
/** TLM PACKET STRUCTS **/
/************************/
typedef struct {
    uint8_t                         Header[CFE_SB_TLM_HDR_SIZE];
    ADCS_HW_DeviceTlmCommon_t       DeviceTlm;
    cubewheel_wheelData_t           WheelTlm[ADCS_WHEEL_COUNT_MAX];
} OS_PACK ADCS_HW_RW_TlmPkt;
#define ADCS_HW_RW_TLM_PKT_LEN sizeof (ADCS_HW_RW_TlmPkt)


/************************/
/** HW Device STRUCTS **/
/************************/

typedef struct {
    uint8_t WheelId;
    cubewheel_telecommand_t* WheelCmd;
} ADCS_RW_Telecommand_t;

typedef struct {
    uint8_t WheelId;
    uint8_t FrameId;
} ADCS_RW_TlmRequest_t;

typedef struct {
    uint8_t         CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint8_t         CmdFlag;
    uint8_t         Count;
    uint8_t         Data[ADCS_RW_TC_CMD_BUFF_COUNT*sizeof(ADCS_RW_Telecommand_t)];
} OS_PACK ADCS_RW_CmdPkt;
#define ADCS_RW_CMD_PKT_LEN sizeof(ADCS_RW_CmdPkt)

typedef struct {
    uint8_t                     ControlMode;
    uint8_t                     WheelCnt;
    // uint32_t                    ChildTaskId;
    // uint32_t                    WakeUpSemaphore;
    // uint32_t                    PollRateMs;
    // boolean                     ChildTaskActive;
    boolean                     Active;
    // boolean                     QueueSetup;
    // TQUEUE_t                    TelecommandQueue;
    // ADCS_RW_Telecommand_t       TelecommandBuffer[ADCS_RW_TC_CMD_BUFF_COUNT];
    // TQUEUE_t                    TlmQueue;
    // ADCS_RW_TlmRequest_t        TlmRequestBuffer[ADCS_RW_TLM_CMD_BUFF_COUNT];
    ADCS_HW_RW_TlmPkt           TlmPkt;
    cubewheel_device_t          Wheel[ADCS_WHEEL_COUNT_MAX];
} ADCS_RW_Class;


/************************/
/** Exported Functions **/
/************************/
int32_t ADCS_HW_RW_Init(ADCS_HW_Device_t *self);
int32_t ADCS_HW_RW_Start(ADCS_HW_Device_t *self);
int32_t ADCS_HW_RW_Stop(ADCS_HW_Device_t *self);
int32_t ADCS_HW_RW_Reset(ADCS_HW_Device_t *self);
int32_t ADCS_HW_RW_ResetStatus(ADCS_HW_Device_t *self);
int32_t ADCS_HW_RW_TLM_Update(ADCS_HW_Device_t *self);
int32_t ADCS_HW_RW_Update(ADCS_HW_Device_t *self);
int32_t ADCS_HW_RW_GetWheelTlm(ADCS_RW_Class *RW, uint8_t wheelId, uint8_t tlmId);
int32_t ADCS_HW_RW_GetIdentificationTlm(ADCS_RW_Class *RW);
int32_t ADCS_HW_RW_PushWheelCmd(ADCS_RW_Class *RW, unsigned int wheelId, unsigned int cmdCount, cubewheel_telecommand_ptr_t* wheelCmd);

#endif