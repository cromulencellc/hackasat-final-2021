#ifndef _BNO08X_DRIVER_H
#define _BNO08X_DRIVER_H

#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>   

#include "sh2.h"
#include "sh2_SensorValue.h"
#include "sh2_hal.h"
#include "shtp.h"
#include "sh2_err.h"

#include "i2c_device.h"
#include "gpio_device.h"
#include "adcs_driver_util.h"
#include "adcs_io_logger.h"

#define BNO08X_I2CADDR_DEFAULT 0x4A ///< The default I2C address
#define I2C_PERIPHERAL_TYPE AXI_I2C
#define BNO08X_DEFAULT_UPDATE_INTERVAL_US 200000

#define SHTP_HEADER_SIZE 4

typedef enum { 
    ACC_UNRELIABLE = 0,
    ACC_LOW = 1,
    ACC_MEDIUM = 2,
    ACC_HIGH = 3
} bno08x_sensor_status_t;

typedef int32_t (*BNO08x_Sleep_FuncPtr) (uint32_t interval_msec);

typedef struct bno08x_sensor_data_t {
    sh2_SensorId_t sensorId;
    uint8_t lastSequence;
    uint8_t sensorValueUnSize;
    uint32_t sensorSequenceMissedCnt;
    uint32_t sensorRecvCnt;
    sh2_SensorConfig_t sensorConfig;
    sh2_SensorValue_t sensorValue;
} BNO08x_Sensor_Data;

typedef struct bno08x_device_t {
    bool active;
    bool deviceReset;
    bool interuptUpdatesEnabled;
    bool sensorsConfigured;
    uint32_t sensorCallbackCnt;
    uint32_t numSensors;
    uint32_t intCounter;
    uint32_t intTimeoutCounter;
    uint32_t intTotalCounter;
    uint64_t refTimeMicros;
    gpio_device_t gpioInterrupt;
    gpio_device_t gpioReset;
    gpio_device_t gpioBoot;
    I2CDevice i2c;
    sh2_Hal_t HAL;
    int (*sleep)(uint32_t msec); // Pointer to function used when sleeping in sensor actions
    BNO08x_Sensor_Data* sensorDataList;
    // double intTimeArray[10000];
} BNO08x_Device;


/*
* Exported Functions
*/
void bno08x_constructor(BNO08x_Device* inputDevice);
void bno08x_init(const char* i2c_bus_name, uint8_t i2c_address, uint8_t i2c_type);
bool bno08x_open(void);
void bno08x_set_interrupt_line(const char* chipname, int line_offset);
void bno08x_set_reset_line(const char* chipname, int line_offset);
void bno08x_set_boot_line(const char* chipname, int line_offset);
void bno08x_enable_interrupt_updates(double interruptTimeoutSeconds);
void bno08x_disable_interrupt_updates(void);
void bno08x_override_sleep_fuction(BNO08x_Sleep_FuncPtr sleepFunc);
void bno08x_setup_reports(BNO08x_Sensor_Data *inputSensorDataList, int numSensors);
void bno08x_init_sensor_data(BNO08x_Sensor_Data *inputSensorData, sh2_SensorId_t sensorId, uint8_t sensorValueUnSize, uint32_t updateInterval);
void bno08x_enable_dynamic_calibration(bool enableAccelerometer, bool enableGyro, bool enableMagnetometer);
void bno08x_save_dynamic_calibration(void);
// void bno08x_run_simple_calibration(uint32_t sensorReportInterval, sh2_CalStatus_t *cal);
int bno08_set_orientation_reference(sh2_Quaternion_t q);
bool bno08x_update(void);
int bno08x_get_sensor_data_index(sh2_SensorId_t id);
const char* bno08x_sensor_status_to_string(uint8_t sensorIdx);
const char* bno08x_sensor_id_to_string(uint8_t sensorId);
// bool bno08x_update_sensor(int sensorIdx, int retryCnt);
void bno08x_close(void);
void bno08x_software_reset(void);
void bno08x_hardware_reset(void);
void bno08x_reset_status(void);

#endif