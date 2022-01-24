#ifndef _FSS_H_
#define _FSS_H_

#include "adcs_hw.h"
#include "adcs_hw_cfg.h"
#include "adcs_conf_tbl.h"


/************************/
/** TLM PACKET STRUCTS **/
/************************/

// TODO: This shouldn't be like this?
// typedef struct fss_device_t { // Looks like MTR driver
//     uint8_t enabled;
// } FssDevice;

typedef enum{
    FSS_SCALAR=0,
    FSS_DISPLACEMENT=1
};

// TODO: Replace this with AcFssType from 42
typedef struct {
    /*~ Parameters ~*/
    double qb[4];
    double CB[3][3];

    /*~ Internal Variables ~*/
    long Valid;       /*>~*/
    double SunAng[2]; /* [[rad]] >~*/
    double SunVecS[3];
    double SunVecB[3];
} OS_PACK FSS_Sensor_Data;

typedef struct {
   uint8_t Valid;
   float SunAng[ADCS_FSS_NUM_ANG];
   float SunVecS[ADCS_FSS_NUM_VEC];
   float SunVecB[ADCS_FSS_NUM_VEC];
} OS_PACK ADCS_HW_FSS_TlmData_t;

typedef struct {
    float    Calibration[3];    // Turn into pointer
    uint8_t  CalibrationType;
} OS_PACK FSS_Cal;

typedef struct {
    uint8_t                   Header[CFE_SB_TLM_HDR_SIZE];
    ADCS_HW_DeviceTlmCommon_t DeviceTlm;
    ADCS_HW_FSS_TlmData_t     FssTlmData;
    FSS_Cal                   Cal;
    // Calibration pointer that copies from other calibration
} OS_PACK ADCS_HW_FSS_TlmSensorPkt;
#define ADCS_HW_FSS_TLM_SENSOR_PKT_LEN sizeof (ADCS_HW_FSS_TlmSensorPkt)


/************************/
/** HW Device STRUCTS **/
/************************/
typedef struct {
    FSS_Sensor_Data           data;
    ADCS_HW_FSS_TlmSensorPkt  TlmPkt;
    uint8_t                   Active;
    FssData_Struct*           ConfTblPtr;
    FSS_Cal*                  Cal;
} OS_PACK ADCS_FSS_Class;
#define ADCS_FSS_CLASS_LEN (sizeof(ADCS_FSS_Class))


/************************/
/** Exported Functions **/
/************************/
int32_t ADCS_HW_FSS_Init(ADCS_HW_Device_t *self);
int32_t ADCS_HW_FSS_Start(ADCS_HW_Device_t *self);
int32_t ADCS_HW_FSS_Stop(ADCS_HW_Device_t *self);
int32_t ADCS_HW_FSS_Reset(ADCS_HW_Device_t *self);
int32_t ADCS_HW_FSS_ResetStatus(ADCS_HW_Device_t *self);
int32_t ADCS_HW_FSS_TLM_Update(ADCS_HW_Device_t *self);
int32_t ADCS_HW_FSS_Update(ADCS_HW_Device_t *self);
int32_t ADCS_HW_FSS_Dump(ADCS_HW_Device_t *self);
// int32_t ADCS_HW_FSS_Dump_old(ADCS_HW_Device_t *self);


#endif //_FSS_H_