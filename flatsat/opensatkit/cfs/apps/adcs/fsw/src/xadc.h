#ifndef _xadc_h_
#define _xadc_h_

// #include "adcs_io_lib.h"
#include "adcs_hw.h"
#include "adcs_hw_cfg.h"
#include "adcs_conf_tbl.h"

/************************/
/** TLM PACKET STRUCTS **/
/************************/

typedef struct {
    uint8_t     Id;
    uint16_t    RawValue;
    double      ConvertedValue;
} OS_PACK ADCS_HW_XADC_TlmData_t;

typedef struct {
    uint8_t                         Header[CFE_SB_TLM_HDR_SIZE];
    ADCS_HW_DeviceTlmCommon_t       DeviceTlm;
    ADCS_HW_XADC_TlmData_t          XadcTlmData[ADCS_XADC_NUM_CHANNELS];
} OS_PACK ADCS_HW_XADC_TlmSensorPkt;
#define ADCS_HW_XADC_TLM_SENSOR_PKT_LEN sizeof (ADCS_HW_XADC_TlmSensorPkt)


/************************/
/** HW Device STRUCTS **/
/************************/

typedef struct {
    xadc_device_t               Device;
    ADCS_HW_XADC_TlmSensorPkt   TlmPkt;
    XadcData_Struct*            ConfTblPtr;
} ADCS_XADC_Class;


/************************/
/** Exported Functions **/
/************************/
int32_t ADCS_HW_XADC_Init(ADCS_HW_Device_t *self);
int32_t ADCS_HW_XADC_Start(ADCS_HW_Device_t *self);
int32_t ADCS_HW_XADC_Stop(ADCS_HW_Device_t *self);
int32_t ADCS_HW_XADC_Reset(ADCS_HW_Device_t *self);
int32_t ADCS_HW_XADC_ResetStatus(ADCS_HW_Device_t *self);
int32_t ADCS_HW_XADC_TLM_Update(ADCS_HW_Device_t *self);
int32_t ADCS_HW_XADC_Update(ADCS_HW_Device_t *self);

#endif