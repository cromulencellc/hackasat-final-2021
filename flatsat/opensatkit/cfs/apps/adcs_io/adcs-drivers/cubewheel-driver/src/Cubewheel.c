
#include "Cubewheel.h"
#include "adcs_driver_util.h"
#include "math.h"

/* 
* File level variables
*/

#define RPM_RAW_CONVERSION_FACTOR 2.0
#define CURRENT_RAW_CONVERSION 0.48828125
#define DUTY_CYCLE_LSB (100.0/INT16_MAX)

/**
 * Local Function Prototypes
 * 
*/

/*
* Local HAL Prototypes
*/
static int i2c_hal_open(cubewheel_device_t *self);
static void i2c_hal_close(cubewheel_device_t *self);
static int i2c_hal_tlm(cubewheel_device_t *self, cubewheel_TelemetryRequestId_t tlmId, uint8_t *data, unsigned int len);
static int i2c_hal_tc(cubewheel_device_t *self, cubewheel_TelecommandId_t tcId, uint8_t *data, unsigned int len);

/*
* Local Cubewheel Prototypes
*/
void cubewheel_i2c_init(cubewheel_device_t* device, const char* i2c_bus_name, uint8_t i2c_address, uint8_t i2c_type);
void cubewheel_gpio_init(cubewheel_device_t* device, const char* chipname, int line_num);
static int get_tlm_size(cubewheel_TelemetryRequestId_t tlmId);
bool tlm_cpy(void* dest, const void* src, const size_t destSize, size_t srcSize);

/******************************************************************************
************************  EXPORTED FUNCTIONS  *********************************
******************************************************************************/

/******************************************************************************
** Function: cubewheel_constructor
**
*/
void cubewheel_constructor(cubewheel_device_t* inputDevice, uint8_t wheelId) {
    memset(inputDevice, 0, sizeof(cubewheel_device_t));
    inputDevice->data.wheelId = wheelId;
        // Setup I2C HAL
    inputDevice->hal.open = i2c_hal_open;
    inputDevice->hal.close = i2c_hal_close;
    inputDevice->hal.sleep = msleep;
    inputDevice->hal.tc = i2c_hal_tc;
    inputDevice->hal.tlm = i2c_hal_tlm;
}

/******************************************************************************
** Function: cubewheel_setup_i2c
**
*/
void cubewheel_i2c_init(cubewheel_device_t* device, const char* i2c_bus_name, uint8_t i2c_address, uint8_t i2c_type) {
    i2c_init(&device->i2c, i2c_type);
    i2c_open(i2c_bus_name, i2c_address, &device->i2c);
}

/******************************************************************************
** Function: cubewheel_setup_gpio
**
*/
void cubewheel_gpio_init(cubewheel_device_t* device, const char* chipname, int line_num) {
    if(!gpio_init_output(&device->gpioEnable, chipname, line_num, "cubewheel_gpio_enable", false, NULL, NULL, GPIO_LOW)) {
        ADCS_IO_LOG_ERROR("Cubewheel setup GPIO failed\n");
    }
}

/******************************************************************************
** Function: cubewheel_init
**
*/
void cubewheel_init(cubewheel_device_t* device, const char* i2c_bus_name, uint8_t i2c_address, uint8_t i2c_type, const char* chipname, int line_num) {
    if (device != NULL) {
        cubewheel_gpio_init(device, chipname, line_num);
        if (device->gpioEnable.active) {
            // cubewheel_hardware_disable(device);
            // device->hal.sleep(100);
            // cubewheel_hardware_enable(device);
            // device->hal.sleep(100);
            cubewheel_hardware_reset(device);
            // cubewheel_hardware_reset(device);
            cubewheel_i2c_init(device, i2c_bus_name, i2c_address, i2c_type);
            if(device->i2c.active) {
                device->data.active = true;
                cubewheel_telecommand_t *tc = cubewheel_tc_reset();
                bool resetStatus = cubewheel_send_tc(device, tc);
                bool indentificationTlmStatus = cubewheel_get_indentification_tlm(device);
                bool periodicTlmStatus = cubewheel_get_periodic_tlm(device);
                bool gainTlmStatus = cubewheel_get_gain_tlm(device);
                if (device->data.wheelStatus.errorFlag != 0 && resetStatus && indentificationTlmStatus && periodicTlmStatus && gainTlmStatus) {
                    device->data.active = false;
                    return;
                }
            }
        }
    }
}

/******************************************************************************
** Function: cubewheel_i2c_begin
**
*/
// void cubewheel_i2c_begin(cubewheel_device_t* device) {
//     if (device != NULL) {
//         device->data.active = false;

//         cubewheel_init(device);
//     }
// }

/******************************************************************************
** Function: cubewheel_override_sleep_fuction
**
*/
void cubewheel_reset_status(cubewheel_device_t* device) {
    if(device != NULL) {
        memset(&device->data, 0, sizeof(cubewheel_wheelData_t));
    }
}

/******************************************************************************
** Function: cubewheel_override_sleep_fuction
**
*/
void cubewheel_override_sleep_fuction(cubewheel_device_t* device, Cubewheel_Sleep_FuncPtr sleepFunc) {
    device->hal.sleep = sleepFunc;
}

/******************************************************************************
** Function: cubewheel_hardware_enable
**
*/
void cubewheel_hardware_enable(cubewheel_device_t* device) {
    ADCS_IO_LOG_DEBUG("Enable Cubewheel. Enable Line -> High\n");
    if(gpio_write(&device->gpioEnable, GPIO_HIGH) != GPIO_SUCCESS ){
        ADCS_IO_LOG_ERROR("Cubewheel GPIO Enable failed");
    };
    device->hal.sleep(150);
    device->data.hardwareEnable = true;
}

/******************************************************************************
** Function: cubewheel_hardware_disable
**
*/
void cubewheel_hardware_disable(cubewheel_device_t* device) {
    ADCS_IO_LOG_DEBUG("Disable Cubewheel. Enable Line -> LOW\n");
    if(gpio_write(&device->gpioEnable, GPIO_LOW) != GPIO_SUCCESS) {
        ADCS_IO_LOG_ERROR("Cubewheel GPIO Disable failed");

    };
    device->hal.sleep(150);
    device->data.hardwareEnable = false;
}

/******************************************************************************
** Function: cubewheel_close
**
*/
void cubewheel_close(cubewheel_device_t* device) {
    device->hal.close(device);
    cubewheel_hardware_disable(device);
    device->data.active = false;
}

/******************************************************************************
** Function: tlm_cpy
**
*/
bool tlm_cpy(void* dest, const void* src, const size_t destSize, size_t srcSize) {
    bool ret = false;
    if (destSize == srcSize) {
        memcpy(dest, src, srcSize);
        ret = true;
    }
    return ret;
}

/******************************************************************************
** Function: cubewheel_get_tlm
**
*/
bool cubewheel_get_tlm(cubewheel_device_t* device, cubewheel_TelemetryRequestId_t tlmId) {
    int tlmSizeRet = 0;
    bool tlmStatus = false;
    if (device != NULL && device->data.active) {
        ADCS_IO_LOG_DEBUG("Get TLM ID %d\n", tlmId);
        int tlmSize = get_tlm_size(tlmId);
        uint8_t tlmBuf[tlmSize];
        tlmSizeRet = device->hal.tlm(device, tlmId, tlmBuf, tlmSize);
        if (tlmSizeRet == tlmSize) {
            switch(tlmId) {
                case TLM_WHEEL_IDENTIFICATION:
                    tlmStatus = tlm_cpy(&device->data.identification, tlmBuf, TLM_IDENTIFICATION_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_EXTENDED_IDENTIFICATION:
                    tlmStatus = tlm_cpy(&device->data.identification, tlmBuf, TLM_IDENTIFICATION_EXT_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_STATUS:
                    tlmStatus = tlm_cpy(&device->data.wheelStatus, tlmBuf, TLM_WHEEL_STATUS_LEN-6, tlmSizeRet-1);
                    device->data.wheelStatus.unused = tlmBuf[7] & 0x7;
                    device->data.wheelStatus.errorFlag = (uint8_t) ((tlmBuf[7]>>3) & 0x1);
                    device->data.wheelStatus.encoderSensorSwitchState = (uint8_t) ((tlmBuf[7]>>4) & 0x1);
                    device->data.wheelStatus.hallSensorSwitchState = (uint8_t) ((tlmBuf[7]>>5) & 0x1);
                    device->data.wheelStatus.motorSwitchState = (uint8_t) ((tlmBuf[7]>>6) & 0x1);
                    device->data.wheelStatus.backupModeState = (uint8_t) ((tlmBuf[7]>>7) & 0x1);
                    break;
                case TLM_WHEEL_SPEED_MEASURED:
                    tlmStatus = tlm_cpy(&device->data.wheelData.wheelSpeed, tlmBuf, TLM_WHEEL_SPEED_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_REFERENCE_SPEED:
                    tlmStatus = tlm_cpy(&device->data.wheelData.wheelReferenceSpeed, tlmBuf, TLM_WHEEL_REFERENCE_SPEED_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_CURRENT_MEASURED:
                    tlmStatus = tlm_cpy(&device->data.wheelData.wheelCurrent, tlmBuf, TLM_WHEEL_CURRENT_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_DATA_COMPLETE:
                    tlmStatus = tlm_cpy(&device->data.wheelData, tlmBuf, TLM_WHEEL_DATA_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_DATA_ADDITIONAL:
                    tlmStatus = tlm_cpy(&device->data.wheelDataAdditional, tlmBuf, TLM_WHEEL_DATA_ADDITIONAL_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_PWM_GAIN:
                    tlmStatus = tlm_cpy(&device->data.pwmGain, tlmBuf, TLM_WHEEL_PWM_GAIN_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_MAIN_GAIN:
                    tlmStatus = tlm_cpy(&device->data.mainGain, tlmBuf, TLM_WHEEL_MAIN_GAIN_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_BACKUP_GAIN:
                    tlmStatus = tlm_cpy(&device->data.backupGain, tlmBuf, TLM_WHEEL_BACKUP_GAIN_LEN, tlmSizeRet);
                    break;
                case TLM_WHEEL_STATUS_FLAGS:
                    if(tlmSizeRet == tlmSize) {
                        device->data.statusFlags.invalidTelemetryFlag = (uint8_t) tlmBuf[0] & 0x1;
                        device->data.statusFlags.invalidTelecommandFlag = (uint8_t) (tlmBuf[0] >> 1) & 0x1;
                        device->data.statusFlags.encoderError = (uint8_t) (tlmBuf[0] >> 2) & 0x1;
                        device->data.statusFlags.uartError = (uint8_t) (tlmBuf[0] >> 3) & 0x1;
                        device->data.statusFlags.i2cError = (uint8_t) (tlmBuf[0] >> 4) & 0x1;
                        device->data.statusFlags.canError = (uint8_t) (tlmBuf[0] >> 5) & 0x1;
                        device->data.statusFlags.configurationError = (uint8_t) (tlmBuf[0] >> 6) & 0x1;
                        device->data.statusFlags.speedError = (uint8_t) (tlmBuf[0] >> 6) & 0x1;
                        tlmStatus = true;
                    }
                    break;
                default:
                    ADCS_IO_LOG_ERROR("Unknown tlm id");
            }
        } else {
            ADCS_IO_LOG_ERROR("TLM Error. TLM ID %d\n", tlmId);
            device->data.tlmErrorCnt++;
        }
    }
 
    return tlmStatus;
}

/******************************************************************************
** Function: cubewheel_get_periodic_tlm
**
*/
bool cubewheel_get_periodic_tlm(cubewheel_device_t* device) {
    bool wheelStatus = cubewheel_get_tlm(device, TLM_WHEEL_STATUS);
    bool wheelDataComplete = cubewheel_get_tlm(device, TLM_WHEEL_DATA_COMPLETE);
    bool wheelDataAddl = cubewheel_get_tlm(device, TLM_WHEEL_DATA_ADDITIONAL);
    bool wheelStatusFlags = cubewheel_get_tlm(device, TLM_WHEEL_STATUS_FLAGS);
    return (wheelStatus && wheelDataComplete && wheelDataAddl && wheelStatusFlags);
}

/******************************************************************************
** Function: cubewheel_get_gain_tlm
**
*/
bool cubewheel_get_gain_tlm(cubewheel_device_t* device) {
    bool pwmGain = cubewheel_get_tlm(device, TLM_WHEEL_PWM_GAIN);
    bool mainGain = cubewheel_get_tlm(device, TLM_WHEEL_MAIN_GAIN);
    bool backupGain = cubewheel_get_tlm(device, TLM_WHEEL_BACKUP_GAIN);
    return (pwmGain && mainGain && backupGain);
}

/******************************************************************************
** Function: cubewheel_get_indentification_tlm
**
*/
bool cubewheel_get_indentification_tlm(cubewheel_device_t* device) {
    bool wheelIdentification = cubewheel_get_tlm(device, TLM_WHEEL_IDENTIFICATION);
    bool wheelIdentificationExt = cubewheel_get_tlm(device, TLM_WHEEL_EXTENDED_IDENTIFICATION);
    return (wheelIdentification && wheelIdentificationExt);
}

/******************************************************************************
** Function: cubewheel_init_telecommand
** Allocates a cubewheel telecommand block. This memory is freed by cubewheel_send_tc.
** Returns ptr to struct. This pointer can then be passed around.
*/
cubewheel_telecommand_t* cubewheel_init_telecommand(void) {
    cubewheel_telecommand_t* tc = calloc(1, sizeof(cubewheel_telecommand_t));
    return tc;
}

/******************************************************************************
** Function: cubewheel_send_tc
**
*/
bool cubewheel_send_tc(cubewheel_device_t* device, cubewheel_telecommand_t *tcData) {
    int tcSizeRet = 0;
    bool status = false; 
    if (device != NULL && device->data.active) {
        device->data.lastCmd = tcData->frameId;
        if (tcData != NULL && device != NULL && device->data.active) {
            ADCS_IO_LOG_DEBUG("Send TC ID %d, Data Len %d", tcData->frameId, tcData->dataLen);
            tcSizeRet = device->hal.tc(device, tcData->frameId, tcData->data, tcData->dataLen);
        }
        if (tcSizeRet != (tcData->dataLen + 1)) {
            ADCS_IO_LOG_ERROR("cubewheel_send_tc() error. TC Frame ID: %d. TC Data Len Desired: %d, TC Data Len Ret: %d", tcData->frameId, tcData->dataLen, tcSizeRet);
            status = false;
            device->data.lastCmdSuccess = false;
            device->data.tcErrorCnt++;
        } else {
            device->data.lastCmdSuccess = true;
            status = true;
        }
    }
    if (tcData != NULL) {
        free(tcData);
    }
    return status;
}

void cubewheel_hardware_reset(cubewheel_device_t* device) {
    if(device != NULL) {
        cubewheel_hardware_disable(device);
        device->hal.sleep(150);
        cubewheel_hardware_enable(device);
    }
}

/******************************************************************************
** Function: cubewheel_tc_reset
**
*/
cubewheel_telecommand_t* cubewheel_tc_reset(void) {
    cubewheel_commandReset_t resetTc;
    resetTc.resetParameter = 85;
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &resetTc, sizeof(cubewheel_commandReset_t));
    tc->dataLen = sizeof(cubewheel_commandReset_t);
    tc->frameId = TC_SET_WHEEL_RESET;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_set_reference_speed
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_reference_speed(float speed) {
    cubewheel_commandWheelReferenceSpeed_t speedTc;
    speedTc.referenceSpeed = rpm_to_raw_value(speed);
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &speedTc, sizeof(cubewheel_commandWheelReferenceSpeed_t));
    tc->dataLen = sizeof(cubewheel_commandWheelReferenceSpeed_t);
    tc->frameId = TC_SET_WHEEL_REFERENCE_SPEED;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_set_duty_cycle
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_duty_cycle(int torqueDutyCycle) {
    cubewheel_commandWheelTorque_t wheeldutyCycle;
    if (torqueDutyCycle >= 100 || torqueDutyCycle <= -100) {
        ADCS_IO_LOG_ERROR("Invalid Duty Cycle: %d\n", torqueDutyCycle);
        return false;
    }
    wheeldutyCycle.dutyCycle = torqueDutyCycle;
    // TODO - Check that this isn't actually encoded to use full scale of integeger
    // if (torqueDutyCycle >= 0.0) {
    //     wheeldutyCycle.dutyCycle = min((int16_t)(torqueDutyCycle * DUTY_CYCLE_LSB), INT16_MAX);
    // } else {
    //     wheeldutyCycle.dutyCycle = max((int16_t)(torqueDutyCycle * DUTY_CYCLE_LSB), INT16_MIN);
    // }
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &wheeldutyCycle, sizeof(cubewheel_commandWheelTorque_t));
    tc->dataLen = sizeof(cubewheel_commandWheelTorque_t);
    tc->frameId = TC_SET_WHEEL_TORQUE;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_set_motor_power_state
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_motor_power_state(bool state) {
    cubewheel_commandMotorPowerState_t motorPwrState;
    motorPwrState.motorPowerEnabledState = (uint8_t) state;
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &motorPwrState, sizeof(cubewheel_commandMotorPowerState_t));
    tc->dataLen = sizeof(cubewheel_commandMotorPowerState_t);
    tc->frameId = TC_SET_WHEEL_MOTOR_POWER_STATE;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_set_encoder_power_state
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_encoder_power_state(bool state) {
    cubewheel_commandEncoderPowerState_t encoderPwrState;
    encoderPwrState.encoderSensorEnabledState = (uint8_t) state;
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &encoderPwrState, sizeof(cubewheel_commandEncoderPowerState_t));
    tc->dataLen = sizeof(cubewheel_commandEncoderPowerState_t);
    tc->frameId = TC_SET_WHEEL_ENCODER_POWER_STATE;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_set_hall_power_state
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_hall_power_state(bool state) {
    cubewheel_commandHallSensorPowerState_t hallPwrState;
    hallPwrState.hallSensorEnabledState = (uint8_t) state;
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &hallPwrState, sizeof(cubewheel_commandHallSensorPowerState_t));
    tc->dataLen = sizeof(cubewheel_commandHallSensorPowerState_t);
    tc->frameId = TC_SET_WHEEL_HALL_POWER_STATE;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_set_control_mode
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_control_mode(uint8_t mode) {
    if (mode < 0 || mode > 3) {
        ADCS_IO_LOG_ERROR("Invalid Control Mode: %d\n", mode);
        return false;
    }
    cubewheel_commandControlMode_t controlMode;
    controlMode.controlMode = mode;
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &controlMode, sizeof(cubewheel_commandControlMode_t));
    tc->dataLen = sizeof(cubewheel_commandControlMode_t);
    tc->frameId = TC_SET_WHEEL_CONTROL_MODE;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_set_backup_mode
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_backup_mode(bool enable) {
    cubewheel_commandBackupMode_t backupMode;
    backupMode.backupMode = (uint8_t) enable;
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &backupMode, sizeof(cubewheel_commandBackupMode_t));
    tc->dataLen = sizeof(cubewheel_commandBackupMode_t);
    tc->frameId = TC_SET_WHEEL_BACKUP_MODE;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_clear_errors
**
*/
cubewheel_telecommand_t* cubewheel_tc_clear_errors(void) {
    cubewheel_commandClearErrors_t clearErrors;
    clearErrors.clearErrorParameter = 85;
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &clearErrors, sizeof(cubewheel_commandClearErrors_t));
    tc->dataLen = sizeof(cubewheel_commandClearErrors_t);
    tc->frameId = TC_SET_WHEEL_CLEAR_ERRORS_FLAG;
    return tc;
}

/******************************************************************************
** Function: cubewheel_tc_set_i2c_address
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_i2c_address(uint8_t addrNew) {
    if (addrNew < 0x8 || addrNew > 0x77) {
        ADCS_IO_LOG_ERROR("Invalid I2C Address: 0x%02X\n", addrNew);
        return NULL;
    }
    cubewheel_commandSetI2CAddr_t setI2CAddress;
    setI2CAddress.i2cAddr = addrNew;
    cubewheel_telecommand_t *tc = cubewheel_init_telecommand();
    memcpy(&tc->data, &setI2CAddress, sizeof(cubewheel_commandSetI2CAddr_t));
    tc->dataLen = sizeof(cubewheel_commandSetI2CAddr_t);
    tc->frameId = TC_SET_WHEEL_I2C_ADDR;
    return tc;
}

/******************************************************************************
** Function: get_new_telecommand
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_pwm_gain(uint16_t K, uint8_t KMultiplier) {
    // TODO - Do we want to implement this
    return NULL;
}

/******************************************************************************
** Function: cubewheel_tc_set_main_gain
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_main_gain(uint16_t Ki, uint8_t KiMultiplier, uint16_t Kd, uint8_t KdMultiplier) {
    // TODO - Do we want to implement this
    return NULL;
}

/******************************************************************************
** Function: cubewheel_tc_set_backup_gain
**
*/
cubewheel_telecommand_t* cubewheel_tc_set_backup_gain(uint16_t Ki, uint8_t KiMultiplier, uint16_t Kd, uint8_t KdMultiplier) {
    // TODO - Do we want to implement this
    return NULL;
}

/******************************************************************************
** Function: cubewheel_get_speed()
**
*/
float cubewheel_get_speed(cubewheel_device_t *device) {
    return raw_value_to_rpm(device->data.wheelData.wheelSpeed);
}

/******************************************************************************
** Function: raw_value_to_rpm
**
*/
float raw_value_to_rpm(int16_t rawValue) {
    return ((float) rawValue) / RPM_RAW_CONVERSION_FACTOR;
}

/******************************************************************************
** Function: rpm_to_raw_value
**
*/
int16_t rpm_to_raw_value(float rpmValue) {
    return (int16_t) (rpmValue * RPM_RAW_CONVERSION_FACTOR);
}

/******************************************************************************
** Function: raw_value_to_current
**
*/
float raw_value_to_current(int16_t rawValue) {
    return ((float) rawValue) * CURRENT_RAW_CONVERSION;
}

/******************************************************************************
** Function: current_to_raw_value
**
*/
uint16_t current_to_raw_value(float current) {
    return (uint16_t) (current/CURRENT_RAW_CONVERSION);
}

/******************************************************************************
** Function: get_tlm_size
**
*/
static int get_tlm_size(cubewheel_TelemetryRequestId_t tlmId) {
    int tlmSize;
    switch(tlmId) {
        case TLM_WHEEL_IDENTIFICATION:
            tlmSize = sizeof(cubewheel_tlmIdentification_t);
            break;
        case TLM_WHEEL_EXTENDED_IDENTIFICATION:
            tlmSize = sizeof(cubewheel_tlmExtendedIdentification_t);
            break;
        case TLM_WHEEL_STATUS:
            tlmSize = 8;
            break;
        case TLM_WHEEL_SPEED_MEASURED:
            tlmSize = sizeof(cubewheel_tlmWheelSpeed_t);
            break;
        case TLM_WHEEL_REFERENCE_SPEED:
            tlmSize = sizeof(cubewheel_tlmWheelReferenceSpeed_t);
            break;
        case TLM_WHEEL_CURRENT_MEASURED:
            tlmSize = sizeof(cubewheel_tlmWheelCurrent_t);
            break;
        case TLM_WHEEL_DATA_COMPLETE:
            tlmSize = sizeof(cubewheel_tlmWheelData_t);
            break;
        case TLM_WHEEL_DATA_ADDITIONAL:
            tlmSize = sizeof(cubewheel_tlmWheelDataAdditional_t);
            break;
        case TLM_WHEEL_PWM_GAIN:
            tlmSize = sizeof(cubewheel_tlmWheelPwmGain_t);
            break;
        case TLM_WHEEL_MAIN_GAIN:
            tlmSize = sizeof(cubewheel_tlmWheelMainGain_t);
            break;
        case TLM_WHEEL_BACKUP_GAIN:
            tlmSize = sizeof(cubewheel_tlmWheelBackupGain_t);
            break;
        case TLM_WHEEL_STATUS_FLAGS:
            tlmSize = 1;
            break;
        default:
            ADCS_IO_LOG_ERROR("Unknown tlm id");
            tlmSize = 0;
    }
    return tlmSize;
}


/******************************************************************************
** Function: i2c_hal_close
**
*/
static void i2c_hal_close(cubewheel_device_t *self) {
    ADCS_IO_LOG_DEBUG("Close I2C\n");
    i2c_close(&self->i2c);
}

/******************************************************************************
** Function: i2c_hal_open
**
*/
static int i2c_hal_open(cubewheel_device_t *self) {

    ADCS_IO_LOG_DEBUG("I2C hal open start\n");
    if(!self->i2c.active) {
        ADCS_IO_LOG_ERROR("Cubewheel setup I2C failed\n");
    }
    ADCS_IO_LOG_DEBUG("I2C hal open successful\n");
    return 0;
}


/******************************************************************************
** Function: i2c_hal_tlm
**
*/
static int i2c_hal_tlm(cubewheel_device_t *self, cubewheel_TelemetryRequestId_t tlmId, uint8_t *data, unsigned int len) {
    ADCS_IO_LOG_DEBUG("I2C HAL get Device TLM %d\n", tlmId);
    int i2cByteCount;
    i2cByteCount = i2c_combined_write_read(&self->i2c, tlmId, data, len);
    return i2cByteCount;
}

/******************************************************************************
** Function: i2c_hal_tc
**
*/
static int i2c_hal_tc(cubewheel_device_t *self, cubewheel_TelecommandId_t tcId, uint8_t *data, unsigned int len) {
    // ADCS_IO_LOG_DEBUG("I2C HAL send TC %d\n", tcId);
    uint8_t tcBuffer[len + 1];
    tcBuffer[0] = tcId;
    memcpy(&tcBuffer[1], data, len);

    if (len > self->i2c.max_write_size) {
        return -1;
    }
    int status = i2c_write(&self->i2c, tcBuffer, len + 1);
    return status;
}

