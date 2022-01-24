#ifndef _CUBEWHEEL_TELECOMMAND_H
#define _CUBEWHEEL_TELECOMMAND_H

#include "Cubewheel_Common.h"


#define CUBEWHEEL_TC_MAX_LEN 6 // sizeof(cubewheel_commandMainGain_t)

/**
 * @brief CubeWheel Communications Structure
 *
 * See Figure 12
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_telecommand_s {
    uint8_t frameId; /**< @brief [uint8_t] Telecommand ID */
    uint8_t dataLen;  /**< @brief [uint8_t] Number of data bytes */
    uint8_t data[CUBEWHEEL_TC_MAX_LEN];  /**< @brief byte array of data */
} cubewheel_telecommand_t;

typedef cubewheel_telecommand_t* cubewheel_telecommand_ptr_t;

/**
 * @brief List of Telemecommand IDs for Cubewheel
 *
 * See Table 13
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef enum cubewheel_TelecommandId_e {
    TC_SET_WHEEL_RESET =                       1,
    TC_SET_WHEEL_REFERENCE_SPEED =             2,
    TC_SET_WHEEL_TORQUE =                      3,
    TC_SET_WHEEL_MOTOR_POWER_STATE =           7,
    TC_SET_WHEEL_ENCODER_POWER_STATE =         8,
    TC_SET_WHEEL_HALL_POWER_STATE =            9,
    TC_SET_WHEEL_CONTROL_MODE =                10,
    TC_SET_WHEEL_BACKUP_MODE =                 12,
    TC_SET_WHEEL_CLEAR_ERRORS_FLAG =           20,
    TC_SET_WHEEL_I2C_ADDR =                    31,
    TC_SET_WHEEL_CAN_MASK =                    32,
    TC_SET_WHEEL_PWM_GAIN =                    33,
    TC_SET_WHEEL_MAIN_GAIN =                   34,
    TC_SET_WHEEL_BACKUP_GAIN =                 35
} cubewheel_TelecommandId_t;



/**
 * @brief CubeWheel Reset Command Definition
 *
 *  See Table 15
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandReset_s {
    uint8_t resetParameter; /**< @brief [bool] Reset parameter (set to decimal 85) */
} cubewheel_commandReset_t;

/**
 * @brief CubeWheel Set Reference Speed Command Defintion
 * raw parameter = ((formatted value))*2.0 (formatted value is in [RPM] units))
 *
 * See Table 16
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandWheelReferenceSpeed_s {
    int16_t referenceSpeed; /**< @brief [RPM/2.0] Reference wheel speed raw value */
} cubewheel_commandWheelReferenceSpeed_t;

/**
 * @brief CubeWheel Set Torque Command Definition
 * The torque provided by the motor will be proportional to the PWM signal. (Unit of measure is [%])
 * 
 * See Table 17
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandWheelTorque_s {
    int16_t dutyCycle; /**< @brief [%] Wheel torque command duty cycle percentage */
} cubewheel_commandWheelTorque_t;

/**
 * @brief CubeWheel Motor Power State Command Definition
 *
 * See Table 18
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandMotorPowerState_s {
    uint8_t motorPowerEnabledState; /**< @brief [bool] Enabled state of battery power to motor */
} cubewheel_commandMotorPowerState_t;

/**
 * @brief CubeWheel Encoder Sensor Power State Command Defition
 *
 * See Table 19
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandEncoderSensorPowerState_s {
    uint8_t encoderSensorEnabledState; /**< @brief [bool] Enabled state of Encoder sensor */
} cubewheel_commandEncoderPowerState_t;

/**
 * @brief CubeWheel Hall Sensor Power State Command Defition
 *
 * See Table 20
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandHallSensorPowerState_s {
    uint8_t hallSensorEnabledState; /**< @brief [bool] Enabled state of Hall sensors */
} cubewheel_commandHallSensorPowerState_t;

/**
 * @brief CubeWheel Control Mode Command Defintion
 * @see cubewheel_Mode_t
 * 
 * See Table 21
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandControlMode_s {
    uint8_t controlMode; /**< @brief [cubewheel_Mode_t] Control mode of motor. */
} cubewheel_commandControlMode_t;

typedef struct cubewheel_commandBackupMode_s {
    uint8_t backupMode; /**< @brief [bool] Enable the backup-mode */
} cubewheel_commandBackupMode_t;

/**
 * @brief CubeWheel Clear Errors Command Definition
 *
 * See Table 24
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandClearErrors_s {
    uint8_t clearErrorParameter;  /**< @brief [uint8_t] Clear error flags paramter (set to decimal 85 to clear the flags) */
} cubewheel_commandClearErrors_t;

/**
 * @brief CubeWheel Set I2C Address Command Definition
 * Note: WAIT 5 seconds for write to EEPROM after command
 * 
 * See Table 25
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandSetI2CAddr_s {
    uint8_t i2cAddr; /**< @brief [uint8_t] New I2C Address */
} cubewheel_commandSetI2CAddr_t;

/**
 * @brief CubeWheel Set PWM Command Definition
 *  Set the general PWM proportional gain
 * 
 * See Table 27
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandPwmGain_s {
    int16_t K; /**< @brief [uint16_t] Main gain paramter */
    uint8_t KMultplier; /**< @brief [uint8_t] Main gain paramter multiplier */
} cubewheel_commandPwmGain_t;

/**
 * @brief CubeWheel Set Main Gain Command Definition
 *  Set the main speed controller gain paramters
 * 
 * See Table 28
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandMainGain_s {
    uint16_t Ki; /**< @brief [uint16_t] Integrator gain paramter */
    uint8_t KiMultplier; /**< @brief [uint8_t] Integrator gain paramter multiplier */
    uint16_t Kd; /**< @brief [uint16_t] Feedback gain paramter */
    uint8_t KdMultplier; /**< @brief [uint8_t] Feedback gain paramter multiplier */
} cubewheel_commandMainGain_t;

/**
 * @brief CubeWheel Set Backup Gain Command Definition
 *  Set the backup speed controller gain paramters
 * 
 * See Table 29
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_commandBackupGain_s {
    uint16_t Ki; /**< @brief [uint16_t] Integrator gain paramter */
    uint8_t KiMultplier; /**< @brief [uint8_t] Integrator gain paramter multiplier */
    uint16_t Kd; /**< @brief [uint16_t] Feedback gain paramter */
    uint8_t KdMultplier; /**< @brief [uint8_t] Feedback gain paramter multiplier */
} cubewheel_commandBackupGain_t;


#endif /* _CUBEWHEEL_TELECOMMAND_H */