
#ifndef _CUBEWHEEL_TELEMETRY_H
#define _CUBEWHEEL_TELEMETRY_H

#include "Cubewheel_Common.h"

/**
 * @brief List of Telemtetry Request IDs for Cubewheel
 *
 * See Table 14
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef enum cubewheel_TelemetryRequestId_e {
    TLM_WHEEL_IDENTIFICATION =            128,
    TLM_WHEEL_EXTENDED_IDENTIFICATION =   129,
    TLM_WHEEL_STATUS =                    130,
    TLM_WHEEL_SPEED_MEASURED =            133,
    TLM_WHEEL_REFERENCE_SPEED =           134,
    TLM_WHEEL_CURRENT_MEASURED =          135, 
    TLM_WHEEL_DATA_COMPLETE =             137,
    TLM_WHEEL_DATA_ADDITIONAL =           138,
    TLM_WHEEL_PWM_GAIN =                  139,
    TLM_WHEEL_MAIN_GAIN =                 140,
    TLM_WHEEL_BACKUP_GAIN =               141,
    TLM_WHEEL_STATUS_FLAGS =              145
} cubewheel_TelemetryRequestId_t;

/**
 * @brief CubeWheel Identification Telemetry Format
 * 
 * See Table 30
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmIdentification_s {
    uint8_t nodeType; /**< @brief [uint] Note type identifier. Will always be 8 for wheel */
    uint8_t interfaceVersion; /**< @brief [uint] Interface version. Should always be 1 */
    uint8_t firmwareVersionMajor; /**< @brief [uint] Major firmware version */
    uint8_t firmwareVersionMinor; /**< @brief [uint] Minor firmware version */
    uint16_t runtimeSeconds; /**< @brief [uint] Runtime in seconds since wheel processor startup */
    uint16_t runtimeMiliseconds; /**< @brief [uint] Runtime miliseconds after current integer second since wheel processor startup */
}  __attribute__((__packed__)) cubewheel_tlmIdentification_t;
#define TLM_IDENTIFICATION_LEN sizeof(cubewheel_tlmIdentification_t)

/**
 * @brief CubeWheel Extended Identification Telemetry Format
 * 
 * See Table 31
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmExtendedIdentification_s {
    uint16_t serialNumber; /**< @brief [uint] Unit serial number */
    uint8_t i2cAddr; /**< @brief [uint] Unit I2C Address */
    uint8_t canAddr; /**< @brief [uint] Unit CAN Address */
}  __attribute__((__packed__)) cubewheel_tlmExtendedIdentification_t;
#define TLM_IDENTIFICATION_EXT_LEN sizeof(cubewheel_tlmExtendedIdentification_t)

/**
 * @brief CubeWheel Wheel Status Telemetry Format
 * 
 * See Table 32
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelStatus_s {
    uint16_t runtimeSeconds; /**< @brief [uint] Runtime in seconds since wheel processor startup */
    uint16_t runtimeMiliseconds; /**< @brief [uint] Runtime miliseconds after current integer second since wheel processor startup */
    uint16_t reserved; /**< @brief [uint] Reserved shows 0 value */
    uint8_t  motorControlMode; /**< @brief [uint] Motor control mode enum cubewheel_Mode_t */
    // uint8_t  statusBitfields; /** */
    uint8_t  backupModeState; /**< @brief [bool] Backup mode state flag */
    uint8_t motorSwitchState; /**< @brief [bool] Motor switch state flag */
    uint8_t hallSensorSwitchState; /**< @brief [bool] Hall sensor switch mode state flag */
    uint8_t encoderSensorSwitchState; /**< @brief [bool] Encoder sensor switch mode state flag */
    uint8_t errorFlag; /**< @brief [bool] Error flag, flag indicates error occured */
    uint8_t unused; /**< @brief [uint] Unused bits */
}  __attribute__((__packed__)) cubewheel_tlmWheelStatus_t;
#define TLM_WHEEL_STATUS_LEN sizeof(cubewheel_tlmWheelStatus_t)

/**
 * @brief CubeWheel Wheel Speed Telemetry Format
 * raw parameter = ((formatted value))*2.0 (formatted value is in [RPM] units))
 * 
 * See Table 33
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelSpeed_s {
    int16_t wheelSpeed; /**< @brief [uint] Raw wheel speed measured */
}  __attribute__((__packed__)) cubewheel_tlmWheelSpeed_t;
#define TLM_WHEEL_SPEED_LEN sizeof(cubewheel_tlmWheelSpeed_t)

/**
 * @brief CubeWheel Wheel Reference Speed Telemetry Format
 * (formatted value) [RPM] = (raw channel value)/2.0
 * 
 * See Table 34
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelReferenceSpeed_s {
    int16_t wheelReferenceSpeed; /**< @brief [uint] Raw wheel speed reference */
} __attribute__((__packed__)) cubewheel_tlmWheelReferenceSpeed_t;
#define TLM_WHEEL_REFERENCE_SPEED_LEN sizeof(cubewheel_tlmWheelReferenceSpeed_t)

/**
 * @brief CubeWheel Wheel Current Telemetry Format
 * (formatted value) [mA] = (raw channel value)*0.48828125
 * 
 * See Table 35
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelCurrent_s {
    int16_t wheelCurrent; /**< @brief [uint] Raw wheel speed reference */
}  __attribute__((__packed__)) cubewheel_tlmWheelCurrent_t;
#define TLM_WHEEL_CURRENT_LEN sizeof(cubewheel_tlmWheelCurrent_t)


/**
 * @brief CubeWheel Wheel Data Telemetry Format
 * Complete Wheel Data
 * 
 * See Table 36
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelData_s {
    int16_t wheelSpeed; /**< @brief [uint] Raw wheel speed measured */
    int16_t wheelReferenceSpeed; /**< @brief [uint] Raw wheel speed reference */
    uint16_t wheelCurrent; /**< @brief [uint] Raw wheel speed reference */
}  __attribute__((__packed__)) cubewheel_tlmWheelData_t;
#define TLM_WHEEL_DATA_LEN sizeof(cubewheel_tlmWheelData_t)

/**
 * @brief CubeWheel Wheel Data Additional Telemetry Format
 * Additional Wheel Data
 * 
 * Wheel Speed: (formatted value) [RPM] = (raw channel value)/2.0
 * 
 * See Table 37
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelDataAdditional_s {
    int16_t wheelDuty; /**< @brief [uint] Current commanded wheel duty cycle */
    int16_t wheelSpeedBackup; /**< @brief [uint] Backup wheel speed measuremnt in rpm */
}  __attribute__((__packed__)) cubewheel_tlmWheelDataAdditional_t;
#define TLM_WHEEL_DATA_ADDITIONAL_LEN sizeof(cubewheel_tlmWheelDataAdditional_t)

/**
 * @brief CubeWheel Wheel PWM Gain Telemetry Format
 * 
 * 
 * See Table 38
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelPwmGain_s {
    int16_t K; /**< @brief [uint16_t] Main gain paramter */
    uint8_t KMultplier; /**< @brief [uint8_t] Main gain paramter multiplier */
}  __attribute__((__packed__)) cubewheel_tlmWheelPwmGain_t;
#define TLM_WHEEL_PWM_GAIN_LEN sizeof(cubewheel_tlmWheelPwmGain_t)


/**
 * @brief CubeWheel Wheel Main Speed Controller Gain Telemetry Format
 * 
 * 
 * See Table 39
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelMainGain_s {
    uint16_t Ki; /**< @brief [uint16_t] Integrator gain paramter */
    uint8_t KiMultplier; /**< @brief [uint8_t] Integrator gain paramter multiplier */
    uint16_t Kd; /**< @brief [uint16_t] Feedback gain paramter */
    uint8_t KdMultplier; /**< @brief [uint8_t] Feedback gain paramter multiplier */
}  __attribute__((__packed__)) cubewheel_tlmWheelMainGain_t;
#define TLM_WHEEL_MAIN_GAIN_LEN sizeof(cubewheel_tlmWheelMainGain_t)

/**
 * @brief CubeWheel Wheel Backup Speed Controller Gain Telemetry Format
 * 
 * 
 * See Table 40
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelBackupGain_s {
    uint16_t Ki; /**< @brief [uint16_t] Integrator gain paramter */
    uint8_t KiMultplier; /**< @brief [uint8_t] Integrator gain paramter multiplier */
    uint16_t Kd; /**< @brief [uint16_t] Feedback gain paramter */
    uint8_t KdMultplier; /**< @brief [uint8_t] Feedback gain paramter multiplier */
}  __attribute__((__packed__)) cubewheel_tlmWheelBackupGain_t;
#define TLM_WHEEL_BACKUP_GAIN_LEN sizeof(cubewheel_tlmWheelBackupGain_t)

/**
 * @brief CubeWheel Wheel Status and Error Telemetry Format
 * 
 * See Table 41
 * Reference CubeWheel - User Manual [v1.15]
 */
typedef struct cubewheel_tlmWheelStatusFlags_s {

    uint8_t invalidTelemetryFlag; /**< @brief [bool] Invalid telemetetry request receieved flag */
    uint8_t invalidTelecommandFlag; /**< @brief [bool] Invalid telecommand request receieved flag */
    uint8_t encoderError; /**< @brief [bool] Encoder error flag */
    uint8_t uartError; /**< @brief [bool] UART protocol error flag */
    uint8_t i2cError; /**< @brief [bool] I2C protocol error flag */
    uint8_t canError; /**< @brief [bool] CAN protocol error flag */
    uint8_t configurationError; /**< @brief [bool] Configuration load error flag */
    uint8_t speedError; /**< @brief [bool] Speed measurements error flag */
}  __attribute__((__packed__)) cubewheel_tlmWheelStatusFlags_t;
#define TLM_WHEEL_STATUS_FLAGS_LEN sizeof(cubewheel_tlmWheelStatusFlags_t)


#endif