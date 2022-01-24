#ifndef _mtr_h_
#define _mtr_h_

// #include "adcs_io_lib.h"
#include "adcs_hw.h"
#include "adcs_hw_cfg.h"
#include "adcs_conf_tbl.h"

/************************/
/** TLM PACKET STRUCTS **/
/************************/

typedef struct {
    uint8_t     Id;
    uint8_t     State;
    uint8_t     GpioPState;
    uint8_t     GpioNState;
    uint16_t    ResetCount;
    uint16_t    StateUpdateCount;
} OS_PACK ADCS_HW_MTR_TlmData_t;

typedef struct {
    uint8_t                         Header[CFE_SB_TLM_HDR_SIZE];
    ADCS_HW_DeviceTlmCommon_t       DeviceTlm;
    ADCS_HW_MTR_TlmData_t           MtrTlmData[ADCS_MTR_NUM_CHANNELS];
} OS_PACK ADCS_HW_MTR_TlmSensorPkt;
#define ADCS_HW_MTR_TLM_SENSOR_PKT_LEN sizeof (ADCS_HW_MTR_TlmSensorPkt)


/************************/
/** HW Device STRUCTS **/
/************************/

typedef struct {
    uint8_t                     MtrCount;
    uint16_t                    MtrResetCnt[ADCS_MTR_NUM_CHANNELS];
    uint16_t                    MtrStateUpdateCnt[ADCS_MTR_NUM_CHANNELS];
    MtrState                    MtrState[ADCS_MTR_NUM_CHANNELS];
    MtrDevice                   Mtr[ADCS_MTR_NUM_CHANNELS];
    ADCS_HW_MTR_TlmSensorPkt    TlmPkt;
    MtrData_Struct*             ConfTblPtr;
} ADCS_MTR_Class;


/************************/
/** Exported Functions **/
/************************/
int32_t ADCS_HW_MTR_Init(ADCS_HW_Device_t *self);
int32_t ADCS_HW_MTR_Start(ADCS_HW_Device_t *self);
int32_t ADCS_HW_MTR_Stop(ADCS_HW_Device_t *self);
int32_t ADCS_HW_MTR_Reset(ADCS_HW_Device_t *self);
int32_t ADCS_HW_MTR_ResetStatus(ADCS_HW_Device_t *self);
int32_t ADCS_HW_MTR_TLM_Update(ADCS_HW_Device_t *self);
int32_t ADCS_HW_MTR_Update(ADCS_HW_Device_t *self);

#endif