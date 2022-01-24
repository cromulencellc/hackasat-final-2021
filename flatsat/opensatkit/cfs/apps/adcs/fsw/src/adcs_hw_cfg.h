
#ifndef _adcs_hw_cfg_h_
#define _adcs_hw_cfg_h_

#include "adcs_io_lib.h"
// #include "TorqueRod.h"
// #include "BNO08x_Driver.h"
// #include "gpio_device.h"
// #include "Cubewheel.h"
// #include "XadcDriver.h"
// #include "uart_st_wrapper.h"

/**
** Global configurations
*/
#define  ADCS_APP_NAME              "ADCS"

// #define _FLATSAT_HARDWARE_FSW_
#define ADCS_CHALLENGE_DEBUG 0
#if ADCS_CHALLENGE_DEBUG
    #define ADCS_CHAL_LOG OS_printf
#else
    #define ADCS_CHAL_LOG(...) while(0){}
#endif
/** 
** IMU Sensor Configuration
*/
#if defined( _FLATSAT_HARDWARE_FSW_)
#define  ADCS_HW_DEVICE_CNT_MAX         6
#define ADCS_HW_DEVICE_IMU_IDX     0
#define ADCS_IMU_NUM_SENSORS        4
#define ADCS_IMU_READ_TIMEOUT_CNT   10
#define ADCS_IMU_POLL_RATE_MS       20
#define ADCS_IMU_UPDATE_RATE        200000
#define ADCS_IMU_GYRO_IDX 0
#define ADCS_IMU_MAG_IDX 2
#define ADCS_IMU_ACC_IDX 1
#define ADCS_IMU_STATE_IDX 3


/** 
** MTR Sensor Configuration
*/
#define ADCS_HW_DEVICE_MTR_IDX  1
#define ADCS_MTR_MAX_CHANNELS   3
#define ADCS_MTR_NUM_CHANNELS   3


/** 
** Cubwheel Configuration
*/
#define ADCS_HW_DEVICE_RW_IDX  2
#define ADCS_WHEEL_COUNT_MAX 3
#define ADCS_WHEEL_COUNT     3
#define ADCS_WHEEL_POLL_RATE_MS 100
#define ADCS_WHEEL_CMD_COUNT_MAX 6

/** 
 * XADC Configuration
 * */
#define ADCS_HW_DEVICE_XADC_IDX 3
#define ADCS_XADC_MAX_NUM_CHANNELS 10
#define ADCS_XADC_NUM_CHANNELS 8

/** 
 * Voltage Monitoring Configuration
 * */
#define ADCS_NUM_ANALOG_MONITOR 3
#define MONITOR_5V_SCALE  (5.22466039707419e-03)
#define MONITOR_5V_XADC_IDX 5
#define MONITOR_3V3_SCALE (0.0035560344827586204)
#define MONITOR_3V3_XADC_IDX 6
#define MONITOR_TEMP_SCALE (0.0001)
#define MONITOR_TEMP_XADC_IDX 7

/**
 * Coarse Sun Sensor Configuration
 * */
#define NUM_CSS_SENSORS 6
#define CSS_CAL_MEASUREMENT_COUNT 10
#define CSS_YAW_0_XADC_IDX      0
#define CSS_YAW_90_XADC_IDX     1
#define CSS_YAW_180_XADC_IDX    2
#define CSS_YAW_270_XADC_IDX    3
#define CSS_Z_MINUS_XADC_IDX    4
#define CSS_Z_XADC_IDX          5


/** 
 * Star Tracker Configuration
 * */
#define ADCS_HW_DEVICE_ST_IDX 4

#elif defined(_FLATSAT_SIM_FSW_)

#define ADCS_WHEEL_COUNT     3
#define ADCS_WHEEL_COUNT_MAX 3
#define ADCS_MTR_MAX_CHANNELS   3
#define ADCS_MTR_NUM_CHANNELS 3
#define ADCS_IMU_NUM_SENSORS 0
#define NUM_CSS_SENSORS 6


#if _FSS_CHALLENGE_ == 1
    #define  ADCS_HW_DEVICE_CNT_MAX         2   // Include FSS Sensor
#else
    #define  ADCS_HW_DEVICE_CNT_MAX         1   // Only include XADC
#endif // _FSS_CHALLENGE
/** 
 * XADC Configuration
 * */
#define ADCS_HW_DEVICE_XADC_IDX 0
#define ADCS_XADC_MAX_NUM_CHANNELS 3
#define ADCS_XADC_NUM_CHANNELS 3

/** 
 * Voltage Monitoring Configuration
 * */
#define ADCS_NUM_ANALOG_MONITOR 3
#define MONITOR_5V_SCALE  (5.22466039707419e-03)
#define MONITOR_5V_XADC_IDX 0
#define MONITOR_3V3_SCALE (0.0035560344827586204)
#define MONITOR_3V3_XADC_IDX 1
#define MONITOR_TEMP_SCALE (0.0001)
#define MONITOR_TEMP_XADC_IDX 2

/**
 * Fine Sun Sensor Configuration
 * ADCS Finals Challenge
 * // _ADCS_CHALLENGE_
 * */
#define NUM_FSS_SENSORS        1

#define ADCS_HW_DEVICE_FSS_IDX 1
#define ADCS_FSS_NUM_ANG       2
#define ADCS_FSS_NUM_VEC       3

#else

#define  ADCS_HW_DEVICE_CNT_MAX         0

#endif

#endif /* _adcs_hw_cfg__h_ */