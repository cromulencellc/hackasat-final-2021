#ifndef _CUBEWHEEL_DRIVER_H
#define _CUBEWHEEL_DRIVER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <limits.h>  


#include "i2c_device.h"
#include "gpio_device.h"
#include "adcs_driver_util.h"
#include "adcs_io_logger.h"

#include "Cubewheel_Common.h"
#include "Cubewheel_Telemetry.h"
#include "Cubewheel_Telecommand.h"

// Uncomment to enable debug
// #define DEBUG_CUBEWHEEL

#define CUBEWHEEL_I2CADDR 0x68 ///< The default I2C address
#define CUBEWHEEL_DEFAULT_I2C_TYPE AXI_I2C

#define CUBEWHEEL_UART_SC       0x1F
#define CUBEWHEEL_UART_SOM      0x7F
#define CUBEWHEEL_UART_EOM      0xFF

#define CUBEWHEEL_CMD_MAX_LEN sizeof(cubewheel_commandMainGain_t)

typedef int32_t (*Cubewheel_Sleep_FuncPtr) (uint32_t interval_msec);

typedef struct cubewheel_hal_s cubewheel_hal_t;
typedef struct cubewheel_device_s cubewheel_device_t;
typedef struct cubewheel_wheelData_s cubewheel_wheelData_t;

/**
 * @brief Cubewheel Telemetry Data Container Struct
 *
 * Container for telemetry for a given wheel telemetry sample
 */
struct __attribute__((__packed__)) cubewheel_wheelData_s {
    uint8_t wheelId;
    uint8_t lastCmd;
    uint8_t lastCmdSuccess;
    uint8_t active;
    uint8_t hardwareEnable;
    uint8_t backupMode;
    uint16_t tcErrorCnt;
    uint16_t tlmErrorCnt;
    cubewheel_tlmIdentification_t identification; 
    cubewheel_tlmExtendedIdentification_t identificationExt;
    cubewheel_tlmWheelStatus_t wheelStatus;
    cubewheel_tlmWheelStatusFlags_t statusFlags;
    cubewheel_tlmWheelData_t wheelData;
    cubewheel_tlmWheelDataAdditional_t wheelDataAdditional;
    cubewheel_tlmWheelPwmGain_t pwmGain;
    cubewheel_tlmWheelMainGain_t mainGain;
    cubewheel_tlmWheelBackupGain_t backupGain;
};

struct cubewheel_hal_s {
    int (*open)(cubewheel_device_t *self);
    void (*close)(cubewheel_device_t *self);
    int (*tlm)(cubewheel_device_t *self, cubewheel_TelemetryRequestId_t tlmId, uint8_t *data, unsigned int len);
    int (*tc)(cubewheel_device_t *self, cubewheel_TelecommandId_t tcId, uint8_t *data, unsigned int len);
    int (*sleep)(uint32_t msec); // Pointer to function used when sleeping in sensor actions
};

struct cubewheel_device_s {
    cubewheel_wheelData_t data;
    cubewheel_hal_t hal;
    I2CDevice i2c;
    gpio_device_t gpioEnable;
};


/*
* Exported Functions
*/
// Setup/init 

/**
* @brief - Constructor function for cubewheel device
* @param inputDevice - pointer to cubewheel_device_t to construct
* @param wheelId - id for wheel to construct
*
*/
void cubewheel_constructor(cubewheel_device_t* inputDevice, uint8_t wheelId);



/**
* @brief - Function to setup I2C for cubewheel device
* @param device - [cubewheel_device_t*] pointer to cubewheel_device_t to perform action
* @param i2c_bus_name - [char*] I2C device name from /dev (e.g. /dev/i2c-2)
* @param i2c_address - [uint8_t] I2C base address for wheel
* @param chipname - [char*] GPIO chipname device name from /sys/bus/gpio/devices
* @param line_number - [uint8_t] GPIO line number for wheel reset 
*
*/
void cubewheel_init(cubewheel_device_t* device, const char* i2c_bus_name, uint8_t i2c_address, uint8_t i2c_type, const char* chipname, int line_num);

// /**
// * @brief - Function to setup GPIO for cubewheel device
// * @param device - [cubewheel_device_t*] pointer to cubewheel_device_t to perform action
// * @param chipname - [char*] GPIO chipname device name from /sys/bus/gpio/devices
// * @param line_number - [uint8_t] GPIO line number for wheel reset 
// *
// */
// void cubewheel_gpio_init(cubewheel_device_t* device, const char* chipname, int line_num);

// /**
// * @brief - Function to startup, initialize and perform basic health check on cubewheel with I2C
// * @param device - [cubewheel_device_t*] pointer to cubewheel_device_t to perform action
// *
// */
// void cubewheel_i2c_begin(cubewheel_device_t* device);
bool cubewheel_update(void);
void cubewheel_hardware_reset(cubewheel_device_t* device);
void cubewheel_reset_status(cubewheel_device_t* device);

/**
* @brief - Function to override HAL sleep function
* @param device - [cubewheel_device_t*] pointer to cubewheel_device_t to perform action
* @param chipname - [char*] GPIO chipname device name from /sys/bus/gpio/devices
* @param line_number - [uint8_t] GPIO line number for wheel reset 
*
*/
void cubewheel_override_sleep_fuction(cubewheel_device_t* device, Cubewheel_Sleep_FuncPtr sleepFunc);
void cubewheel_hardware_enable(cubewheel_device_t* device);
void cubewheel_hardware_disable(cubewheel_device_t* device);
void cubewheel_close(cubewheel_device_t* device);
// void cubewheel_software_reset(void);

// Cubewheel Telecommands
cubewheel_telecommand_t* cubewheel_init_telecommand(void);
bool cubewheel_send_tc(cubewheel_device_t* device, cubewheel_telecommand_t *tcData);
cubewheel_telecommand_t* cubewheel_tc_reset(void);
cubewheel_telecommand_t* cubewheel_tc_set_reference_speed(float speed);
cubewheel_telecommand_t* cubewheel_tc_set_duty_cycle(int torqueDutyCycle);
cubewheel_telecommand_t* cubewheel_tc_set_motor_power_state(bool state);
cubewheel_telecommand_t* cubewheel_tc_set_encoder_power_state(bool state);
cubewheel_telecommand_t* cubewheel_tc_set_hall_power_state(bool state);
cubewheel_telecommand_t* cubewheel_tc_set_control_mode(uint8_t mode);
cubewheel_telecommand_t* cubewheel_tc_set_backup_mode(bool enable);
cubewheel_telecommand_t* cubewheel_tc_clear_errors(void);
cubewheel_telecommand_t* cubewheel_tc_set_i2c_address(uint8_t addrNew);
cubewheel_telecommand_t* cubewheel_tc_set_pwm_gain(uint16_t K, uint8_t KMultiplier);
cubewheel_telecommand_t* cubewheel_tc_set_main_gain(uint16_t Ki, uint8_t KiMultiplier, uint16_t Kd, uint8_t KdMultiplier);
cubewheel_telecommand_t* cubewheel_tc_set_backup_gain(uint16_t Ki, uint8_t KiMultiplier, uint16_t Kd, uint8_t KdMultiplier);

// Cubewheel Telemetry
bool cubewheel_get_tlm(cubewheel_device_t* device, cubewheel_TelemetryRequestId_t tlmId);
bool cubewheel_get_periodic_tlm(cubewheel_device_t* device);
bool cubewheel_get_indentification_tlm(cubewheel_device_t* device);
bool cubewheel_get_gain_tlm(cubewheel_device_t* device);
float cubewheel_get_speed(cubewheel_device_t *device);


// Utility 
float raw_value_to_rpm(int16_t rawValue);
int16_t rpm_to_raw_value(float rpmValue);
float raw_value_to_current(int16_t rawValue);
uint16_t current_to_raw_value(float current);

#endif /* _CUBEWHEEL_DRIVER_H */