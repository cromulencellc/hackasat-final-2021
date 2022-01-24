#ifndef _imu_h_
#define _imu_h_

// #include "adcs_io_lib.h"
#include "adcs_hw.h"
#include "adcs_conf_tbl.h"
#include "adcs_hw_cfg.h"

#define ADCS_IMU_CHILD_SEM_NAME     "IMU_CHILD_SEM"
#define ADCS_IMU_CHILD_NAME         "IMU_CHILD"
#define ADCS_IMU_STACK_SIZE         8096
#define ADCS_IMU_CHILD_PRIORITY     40
#define ADCS_IMU_SEM_INVALID        0xFFFFFFFF
#define ADCS_IMU_SENSOR_DATA_BUFFER (sizeof(float) * 5)

/************************/
/** TLM PACKET STRUCTS **/
/************************/

typedef struct {
    uint8_t  sensorId;
    uint8_t  sensorSequence;
    uint8_t  lastSensorSequence;
    uint8_t  sensorStatus;
    uint32_t sensorRecvCnt;
    uint32_t sensorSequenceMissedCnt;
    uint32_t sensorDelay;
    uint64_t sensorTimestamp;
    uint8_t  sensorData[ADCS_IMU_SENSOR_DATA_BUFFER];
} OS_PACK ADCS_HW_IMU_SensorDataOutput_t;

typedef struct {
    uint8_t                         Header[CFE_SB_TLM_HDR_SIZE];
    // uint32_t                        Pad;
    ADCS_HW_DeviceTlmCommon_t       DeviceTlm;
    ADCS_HW_IMU_SensorDataOutput_t  SensorData[ADCS_IMU_NUM_SENSORS];
} OS_PACK ADCS_HW_IMU_TlmSensorPkt;
#define ADCS_HW_IMU_TLM_SENSOR_PKT_LEN sizeof (ADCS_HW_IMU_TlmSensorPkt)


/************************/
/** HW Device STRUCTS **/
/************************/

typedef struct {
    uint32_t                    ChildTaskId;
    uint32_t                    WakeUpSemaphore;
    uint32_t                    PollRateMs;
    boolean                     ChildTaskActive;
    boolean                     InterruptsEnabled;
    uint8_t                     SensorCount;
    ImuData_Struct*             ConfTblPtr;
    BNO08x_Device               Device;
    ADCS_HW_IMU_TlmSensorPkt    TlmPkt;
    BNO08x_Sensor_Data          Data[ADCS_IMU_NUM_SENSORS];
} ADCS_IMU_Class;


/************************/
/** Exported Functions **/
/************************/
int32_t ADCS_HW_IMU_Init(ADCS_HW_Device_t *self);
int32_t ADCS_HW_IMU_Start(ADCS_HW_Device_t *self);
int32_t ADCS_HW_IMU_Stop(ADCS_HW_Device_t *self);
int32_t ADCS_HW_IMU_Reset(ADCS_HW_Device_t *self);
int32_t ADCS_HW_IMU_ResetStatus(ADCS_HW_Device_t *self);
int32_t ADCS_HW_IMU_TLM_Update(ADCS_HW_Device_t *self);
int32_t ADCS_HW_IMU_Update(ADCS_HW_Device_t *self);

#endif