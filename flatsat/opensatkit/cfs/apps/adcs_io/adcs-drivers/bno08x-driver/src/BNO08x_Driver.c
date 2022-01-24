
#include <math.h>
#include "BNO08x_Driver.h"
/* 
* File level variables
*/
static BNO08x_Device* device = NULL;
static sh2_SensorValue_t sensorValue;
static uint8_t softreset_pkt[] = {5, 0, 1, 0, 1};

/*
* Local HAL Function Prototypes
*/
static int i2c_hal_write(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len);
static int i2c_hal_read(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len, uint32_t *t_us);
static void i2c_hal_close(sh2_Hal_t *self);
static int i2c_hal_open(sh2_Hal_t *self);
bool driver_open(void);
int device_sleep(uint32_t interval_msec);
static void hal_software_reset();
static void hal_hardware_reset();
static void sensor_event_handler(void *cookie, sh2_SensorEvent_t *event);
static void hal_event_callback(void *cookie, sh2_AsyncEvent_t *pEvent);
static uint32_t hal_get_micros(sh2_Hal_t *self);
int hal_interrupt_callback(int event_type, struct timespec *event_time);

/******************************************************************************
************************  EXPORTED FUNCTIONS  *********************************
******************************************************************************/

/******************************************************************************
** Function: bno08x_init
**
*/
void bno08x_constructor(BNO08x_Device* inputDevice) {
    device = inputDevice;                 
    memset(device, 0, sizeof(BNO08x_Device));
    device->sleep = msleep;
}

/******************************************************************************
** Function: bno08x_init
**
*/
void bno08x_init(const char* i2c_bus_name, uint8_t i2c_address, uint8_t i2c_type) {
    if (device != NULL) {
        device->HAL.open = i2c_hal_open;
        device->HAL.close = i2c_hal_close;
        device->HAL.read = i2c_hal_read;
        device->HAL.write =  i2c_hal_write;
        device->HAL.getTimeUs = hal_get_micros;
        device->refTimeMicros = 0;

        // Use system sleep by default
        if (device->sleep == NULL) {
            device->sleep = msleep;
        }

        i2c_init(&device->i2c, i2c_type);
        i2c_open(i2c_bus_name, i2c_address, &device->i2c);

    }
}

bool bno08x_open(void) {
    bool status = false;
    if(device != NULL && device->i2c.active) {
        if((status = driver_open()) == true) {
            device->active = true;
            status =  true;
        } else {
            ADCS_IO_LOG_ERROR("Driver init failed");
            device->active = false;
        }
    }
    return status;
}

void bno08x_set_interrupt_line(const char* chipname, int line_offset) {
    if(!gpio_init_input(&device->gpioInterrupt, chipname, line_offset, "bno08x_interrupt", true, NULL)) {
        ADCS_IO_LOG_ERROR("Interrupt GPIO Init Failed");
    }
    device->gpioInterrupt.active = true;
    gpio_register_event_callback(&device->gpioInterrupt, (gpio_event_callback) &hal_interrupt_callback);
}

void bno08x_enable_interrupt_updates(double interruptTimeoutSeconds) {
    device->interuptUpdatesEnabled = true;
    if(gpio_mon_falling_edge(&device->gpioInterrupt, interruptTimeoutSeconds) != GPIO_SUCCESS) {
        ADCS_IO_LOG_ERROR("BNO08x Error Setting Up Interupt Monitoring");
    }
    

}

void bno08x_disable_interrupt_updates(void) {
    device->interuptUpdatesEnabled = false;
}

void bno08x_set_reset_line(const char* chipname, int line_offset) {
    if(!gpio_init_output(&device->gpioReset, chipname, line_offset, "bno08x_reset", false, "pull-up","open-source", 1)) {
        ADCS_IO_LOG_ERROR("Interrupt GPIO Init Failed");
    }
    device->gpioReset.active = true;
}

void bno08x_set_boot_line(const char* chipname, int line_offset) {
    if(!gpio_init_output(&device->gpioBoot, chipname, line_offset, "bno08x_boot", false, "pull-up","open-source", 1)) {
        ADCS_IO_LOG_ERROR("BOOT GPIO Init Failed");
    }
    device->gpioBoot.active = true;
}

/******************************************************************************
** Function: bno08x_run_simple_calibration
**
*/
void bno08x_override_sleep_fuction(BNO08x_Sleep_FuncPtr sleepFunc) {
    if(sleepFunc != NULL) {
        device->sleep = sleepFunc;   
    }
}
    
/******************************************************************************
** Function: bno08x_init_default_sensor_data
**
*/
void bno08x_init_sensor_data(BNO08x_Sensor_Data *inputSensorData, sh2_SensorId_t sensorId, uint8_t sensorValueUnSize, uint32_t updateInterval) {
    inputSensorData->sensorId = sensorId;
    inputSensorData->sensorValueUnSize = sensorValueUnSize;
    inputSensorData->sensorRecvCnt = 0;
    inputSensorData->sensorConfig.changeSensitivityEnabled = false;
    inputSensorData->sensorConfig.wakeupEnabled = false;
    inputSensorData->sensorConfig.changeSensitivityRelative = false;
    inputSensorData->sensorConfig.alwaysOnEnabled = false;
    inputSensorData->sensorConfig.changeSensitivity = 0;
    inputSensorData->sensorConfig.batchInterval_us = 0;
    inputSensorData->sensorConfig.sensorSpecific = 0;
    inputSensorData->sensorConfig.reportInterval_us = updateInterval;
}


/******************************************************************************
** Function: bno08x_setup_report
**
*/
void bno08x_setup_reports(BNO08x_Sensor_Data *inputSensorDataList, int numSensors) {
    device->numSensors = numSensors;
    device->sensorDataList = inputSensorDataList;
    // Loop over list of sensor reports and set them up
    int sleepTime = 0;
    if (device->active) {
        for (int i=0; i<device->numSensors; i++) {
            ADCS_IO_LOG_DEBUG("Enable SensorId %d in device idx: %d", device->sensorDataList[i].sensorId, i);
            if(sh2_setSensorConfig(device->sensorDataList[i].sensorId, &device->sensorDataList[i].sensorConfig) != SH2_OK) {
                ADCS_IO_LOG_ERROR("Error setting up reports");
                device->active = false;
            }
            sleepTime = (device->sensorDataList[i].sensorConfig.reportInterval_us * 0.1) / 1000;
            ADCS_IO_LOG_DEBUG("Sleep %d msec before next sensor config", sleepTime);
            device->sleep(sleepTime);
        }
    }
    if(device->active) {
        device->sensorsConfigured = true;
    }
}

/******************************************************************************
** Function: bno08x_enable_dynamic_calibration
**
*/
void bno08x_enable_dynamic_calibration(bool enableAccelerometer, bool enableGyro, bool enableMagnetometer) {
    uint8_t sensorBitmask = 0;
    if (enableAccelerometer) {
        sensorBitmask |= SH2_CAL_ACCEL;
    }
    if (enableGyro) {
        sensorBitmask |= SH2_CAL_GYRO;
    }
    if (enableMagnetometer) {
        sensorBitmask |= SH2_CAL_MAG;
    }
    ADCS_IO_LOG_INFO("Setting dynamic calibration settings %d. Accel: %d, Gyro: %d, Magnetometer: %d", sensorBitmask, enableAccelerometer, enableGyro, enableMagnetometer);
    if(device->active) {
        if(sh2_setCalConfig(sensorBitmask) != SH2_OK) {
            ADCS_IO_LOG_ERROR("Error setting dynamic calibration config");
        }
    }
}

/******************************************************************************
** Function: bno08x_save_dynamic_calibration
**
*/
void bno08x_save_dynamic_calibration(void) {
    ADCS_IO_LOG_INFO("Saving dynamic calibration settings");
    if(device->active) {
        if(sh2_saveDcdNow() != SH2_OK) {
            ADCS_IO_LOG_ERROR("Error setting dynamic calibration config");
        }
    }
}

/******************************************************************************
** Function: bno08x_save_dynamic_calibration
**
*/
// void bno08x_load_dynamic_calibration(void) {
//     printf("Saving dynamic calibration settings.\n");
//     if(device->active) {
//         if(sh2_clearCounts() != SH2_OK) {
//             ERROR_PRINT("Error setting dynamic calibration config");
//         }
//     }
// }

// /******************************************************************************
// ** Function: bno08x_run_simple_calibration
// **
// */
// void bno08x_run_simple_calibration(uint32_t sensorReportInterval, sh2_CalStatus_t *cal) {
//     if(device->active) {
//         ADCS_IO_LOG_DEBUG("Running BNO08X Simple Cal");
//         if(sh2_startCal(sensorReportInterval) != SH2_OK)  {
//             ADCS_IO_LOG_DEBUG("Error running simple calibration");
//         };
//         device_sleep(100);
//         if(sh2_finishCal(cal) != SH2_OK) {
//             ADCS_IO_LOG_DEBUG("Couldn't retrieve simple calibration data");
//         }
//     }
// }


/******************************************************************************
** Function: bno08x_set_orientation_reference
**
*/
int bno08x_set_orientation_reference(sh2_Quaternion_t q) {
    int ret = 1;
    if (device->active) {
        ADCS_IO_LOG_DEBUG("Set orientation reference: Qx: %5.4f, Qy: %5.4f, Qz: %5.4f, Qw: %5.4f", q.x, q.y, q.z, q.w);
        ret = sh2_setReorientation(&q);
    }
    return ret;
}


/******************************************************************************
** Function: bno08x_setup_report
**
*/
bool bno08x_update(void) {
    bool ret = true;
    sensorValue.timestamp = 0;
    if (device->active) {
        sh2_service();
        if (sensorValue.timestamp == 0) {
            // no new events
            ret = false;
        }
        // ret = bno08x_get_sensor_data_index(sensorValue.sensorId);
        // device->sensorDataList[ret].sensorRecvCnt++;
        // memcpy(&device->sensorDataList[ret].sensorValue, &sensorValue, sizeof(sh2_SensorValue_t));
    }
    return ret;
}

/******************************************************************************
** Function: bno08x_setup_report
**
*/
// bool bno08x_update_sensor(int sensorIdx, int retryCnt) {
//     int ret;
//     bool status;
//     int retry = 0;
//     sensorValue.timestamp = 0;
//     if (device->active) {
//         ret = sh2_flush(device->sensorDataList[sensorIdx].sensorId);
//         while(ret < 0 && retry < retryCnt) {
//             retry++;
//             ret = sh2_flush(device->sensorDataList[sensorIdx].sensorId);
//         } 
//         int sensorIdUpdate;
//         retry = 0;
//         while(retry < retryCnt) {
//             if (sensorValue.sensorId == device->sensorDataList[sensorIdx].sensorId)
//                 break;
//             retry++;
//             msleep(20);
//         } 
        
//         if (sensorValue.sensorId == device->sensorDataList[sensorIdx].sensorId) {
//             ADCS_IO_LOG_DEBUG("Successfully flushed sensorId: %d after %d retry", device->sensorDataList[sensorIdx].sensorId, retry);
//             status = true;
//         } else {
//             ADCS_IO_LOG_DEBUG("Sensor flush timeout... :( sensorId: %d", device->sensorDataList[sensorIdx].sensorId);
//             status = false;
//         }

//     }
//     return status;
// }

/******************************************************************************
** Function: bno08x_get_sensor_data_ptr
**
*/
int bno08x_get_sensor_data_index(sh2_SensorId_t id) {
    int idx = 0;
    while(idx < device->numSensors) {
        if (device->sensorDataList[idx].sensorId == id) {
            return idx;
        }
        idx++;
    }
    return -1;
}

/******************************************************************************
** Function: bno08x_close
**
*/
void bno08x_close(void) {
    ADCS_IO_LOG_DEBUG("Closing IMU and I2C connections");
    if (device->active) {
        sh2_close();
    }
    device->active = false;
}

/******************************************************************************
** Function: bno08xset_orientation_reference
**
*/
void bno08x_software_reset(void) {
    hal_software_reset();
    bno08x_reset_status();
    device->deviceReset = false;
}


/******************************************************************************
** Function: hardware_reset
**
*/
void bno08x_hardware_reset(void) {
    hal_hardware_reset();
    bno08x_reset_status();
}   

/******************************************************************************
** Function: bno08x_reset_status
**
*/
void bno08x_reset_status(void) {
    device->deviceReset = false;
    device->interuptUpdatesEnabled = false;
    device->intCounter = 0;
    device->intTotalCounter = 0;
    device->intTimeoutCounter = 0;
    device->sensorsConfigured = false;
    if (device->sensorDataList != NULL) {
        for(int i=0; i<device->numSensors; i++) {
            device->sensorDataList[i].sensorRecvCnt = 0;
            device->sensorDataList[i].sensorSequenceMissedCnt = 0;
        }
    }
}

/******************************************************************************
** Function: bno08x_sensor_status_to_string
**
*/
const char* bno08x_sensor_status_to_string(uint8_t sensorIdx) {
    uint8_t statusValue = device->sensorDataList[sensorIdx].sensorValue.status & 0x3;
    switch(statusValue) {
        case ACC_UNRELIABLE:
            return "UNRELIABLE";
        case ACC_LOW:
            return "LOW";
        case ACC_MEDIUM:
            return "MEDIUM";
        case ACC_HIGH:
            return "HIGH";
        default:
            ADCS_IO_LOG_ERROR("Unknown status value %d", statusValue);
            return NULL;
    }
}

/******************************************************************************
** Function: bno08x_sensor_status_to_string
**
*/
const char* bno08x_sensor_id_to_string(uint8_t sensorId) {
    switch(sensorId) {
        case SH2_GYROSCOPE_CALIBRATED:
            return "GYRO_CALIBRATED";
        case SH2_ROTATION_VECTOR:
            return "ROTATION_VECTOR";
        case SH2_ACCELEROMETER:
            return "ACCELEROMETER_CALIBRATED";
        case SH2_MAGNETIC_FIELD_CALIBRATED:
            return "MAGNETOMETER_CALIBRATED";
        case SH2_GAME_ROTATION_VECTOR:
            return "GAME_ROTATION_VECTOR";
        default:
            ADCS_IO_LOG_ERROR("Unknown sensorId value %d", sensorId);
            return NULL;
    }
}

/******************************************************************************
************************  LOCAL FUNCTIONS  ************************************
******************************************************************************/

int device_sleep(uint32_t interval_msec) {
    if (device->sleep != NULL) {
        return device->sleep(interval_msec);
    }
    else {
        return -1;
    }
}


/******************************************************************************
** Function: driver_open
**
*/
bool driver_open(void) {
    int status;

    // TODO Implement hardware reset if needed (more GPIO requireds)
    device->refTimeMicros = get_micros_32();

    // Open SH2 interface (also registers non-sensor event handler.)
    ADCS_IO_LOG_DEBUG("sh2_open");
    status = sh2_open(&device->HAL, hal_event_callback, NULL);
    if (status != SH2_OK) {
        return false;
    }
    // hal_software_reset();

    sh2_ProductIds_t prodIds;
    // Check connection partially by getting the product id's
    memset(&prodIds, 0, sizeof(prodIds));
    // ADCS_IO_LOG_DEBUG("sh2_getProdIds\n");
    status = sh2_getProdIds(&prodIds);
    ADCS_IO_LOG_DEBUG("Product IDs. Part No:%d, SW Major: %d, Minor: %d ,Patch: %d", 
        prodIds.entry[0].swPartNumber,
        prodIds.entry[0].swVersionMajor,
        prodIds.entry[0].swVersionMinor,
        prodIds.entry[0].swVersionPatch);
    if (status != SH2_OK) {
        return false;
    }
    ADCS_IO_LOG_DEBUG("Driver init successful, register sensor event handler");
    // Register sensor listener
    sh2_setSensorCallback(sensor_event_handler, NULL);

    return true;
}

/******************************************************************************
** Function: i2c_hal_close
**
*/
static void i2c_hal_close(sh2_Hal_t *self) {
    ADCS_IO_LOG_DEBUG("Close I2C");
    i2c_close(&device->i2c);
}

/******************************************************************************
** Function: i2c_hal_open
**
*/
static int i2c_hal_open(sh2_Hal_t *self) {

    ADCS_IO_LOG_DEBUG("I2C hal open");
    i2c_write(&device->i2c, softreset_pkt, 5);
    device_sleep(200);
    i2c_write(&device->i2c, softreset_pkt, 5);
    device_sleep(200);
    // hal_software_reset();
    // ADCS_IO_LOG_DEBUG("I2C hal open successful");
    return 0;
}

/******************************************************************************
** Function: i2c_hal_read
**
*/
static int i2c_hal_read(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len, uint32_t *t_us) {
    
    uint8_t header[SHTP_HEADER_SIZE];
    uintptr_t copyPtr = (uintptr_t) pBuffer;

    // Read SHTP header to get payload cargo size
    if(i2c_read(&device->i2c, header, 4) < 0) {
        return 0;
    }

    // Use byte shifts from SH2 code
    unsigned int payload_size = (header[0] + (header[1] << 8)) & (~0x8000);
    // unsigned int continuation = ((header[1] & 0x80) != 0);
    // uint channel = header[2];
    // uint sequence = header[3];
    
    if (payload_size > len) {
        ADCS_IO_LOG_DEBUG("Payload size excedes passed length");
        return 0;

    }

    unsigned int data_bytes_remaining = payload_size;
    unsigned int data_bytes_read = 0;
    bool first = true;
    uint8_t i2c_read_buffer[device->i2c.max_read_size];
    int i2c_read_size = 0;
    while (data_bytes_remaining > 0) {
        if (first) {
            i2c_read_size = min(device->i2c.max_read_size, data_bytes_remaining);
        } else {
            i2c_read_size = min(device->i2c.max_read_size, data_bytes_remaining + SHTP_HEADER_SIZE);
        }

        if(i2c_read(&device->i2c, i2c_read_buffer, i2c_read_size) < 0) {
            return 0;
        }

        if (first) {
            data_bytes_read = i2c_read_size;
            memcpy((void *)copyPtr, i2c_read_buffer, data_bytes_read);
            first = false;
        } else {
            data_bytes_read = i2c_read_size - SHTP_HEADER_SIZE;
            memcpy((void *)copyPtr, i2c_read_buffer+SHTP_HEADER_SIZE, data_bytes_read);
        }
        
        copyPtr += i2c_read_size;
        data_bytes_remaining -= data_bytes_read;
    }
    *t_us = get_micros_32();
    return payload_size;

}

/******************************************************************************
** Function: i2c_hal_write
**
*/
static int i2c_hal_write(sh2_Hal_t *self, uint8_t *pBuffer, unsigned len) {
    
    if (len > device->i2c.max_write_size) {
        return -1;
    }
    
    int status = i2c_write(&device->i2c, pBuffer, len);
    // Return number of bytes written
    return status;
}


/******************************************************************************
** Function: hal_event_callback
**
*/
static void hal_event_callback(void *cookie, sh2_AsyncEvent_t *pEvent) {
  // If we see a reset, set a flag so that sensors will be reconfigured.
  if (pEvent->eventId == SH2_RESET) {
    // Serial.println("Reset!");
    device->deviceReset = true;
  } else {
      ADCS_IO_LOG_DEBUG("Event ID %d Recieved\n", pEvent->eventId);
  }
}


/******************************************************************************
** Function: sensor_event_handler
**
*/
static void sensor_event_handler(void *cookie, sh2_SensorEvent_t *event) {
  int rc;
  rc = sh2_decodeSensorEvent(&sensorValue, event);
  if (rc != SH2_OK) {
    ADCS_IO_LOG_DEBUG("BNO08x - Error decoding sensor event");
    sensorValue.timestamp = 0.0;
    return;
  } else {
        int id = bno08x_get_sensor_data_index(sensorValue.sensorId);
        device->sensorCallbackCnt++;
        bool update = true;
        unsigned int countMissed = 0;
        if (device->sensorDataList[id].lastSequence == sensorValue.sequence) {
            update = false;
        }
        if (device->sensorDataList[id].lastSequence > sensorValue.sequence ) {
            countMissed = (int) fabs(device->sensorDataList[id].lastSequence - sensorValue.sequence - 256);
        } else {
            if (sensorValue.sequence == 0 && device->sensorDataList[id].lastSequence == 0 ) {
                countMissed = 0;
            } else {
                countMissed = sensorValue.sequence - device->sensorDataList[id].lastSequence - 1;
            }
        }
        if (update) {
            device->sensorDataList[id].lastSequence = sensorValue.sequence;
            device->sensorDataList[id].sensorRecvCnt++;
            device->sensorDataList[id].sensorSequenceMissedCnt += countMissed;
            memcpy(&device->sensorDataList[id].sensorValue, &sensorValue, sizeof(sh2_SensorValue_t));
        }
        ADCS_IO_LOG_DEBUG("Sensor event ID %d, Timestamp %lld, Sequence Counter %u, Counts Missed %u", id, sensorValue.timestamp, sensorValue.sequence, device->sensorDataList[id].sensorSequenceMissedCnt);
  }
}

/******************************************************************************
** Function: hal_get_micros
**
*/
static uint32_t hal_get_micros(sh2_Hal_t *self) {
    return get_micros_32() - device->refTimeMicros;
}

/******************************************************************************
** Function: hal_hardware_reset
** TODO fix hardware reset (look at datasheet again)
*/
static void hal_hardware_reset() {
    if (device->gpioReset.active) {
        if (device->active) {
            bno08x_close();
        }
        gpio_write(&device->gpioReset, 0);
        device_sleep(500);
        gpio_write(&device->gpioReset, 1);
        device_sleep(500);
    } else {
        ADCS_IO_LOG_DEBUG("HW Reset not implemented");
    }
    
}

/******************************************************************************
** Function: hal_software_reset
**
*/
static void hal_software_reset() {
    ADCS_IO_LOG_INFO("I2C hal software reset");
    sh2_devReset();
    // i2c_write(&device->i2c, softreset_pkt, 5);
    device_sleep(200);
}


int hal_interrupt_callback(int event_type, struct timespec *event_time) {
    // clock_t start, end;
    // double cpu_time_used;
    // start = clock();

    // bool updateStatus = false;
    int ret = GPIOD_CTXLESS_EVENT_CB_RET_OK;
    if (event_type == GPIOD_CTXLESS_EVENT_CB_TIMEOUT) {
        // updateStatus = true;
        // while(updateStatus) {
        //     updateStatus = bno08x_update();
        // }
        ADCS_IO_LOG_DEBUG("INT Timeout");
        device->intTimeoutCounter++;
        
    } else if (event_type == GPIOD_CTXLESS_EVENT_CB_FALLING_EDGE) {
        bno08x_update();
        ADCS_IO_LOG_DEBUG("INT Counter %d, device->intCounter");
        device->intCounter++;
    }
    if(!device->interuptUpdatesEnabled) {
        ADCS_IO_LOG_DEBUG("Stopping Interrupt handler");
        ret = GPIOD_CTXLESS_EVENT_CB_RET_STOP;
    }
    // end = clock();
    // cpu_time_used = ((double)(end-start)) / CLOCKS_PER_SEC;
    // device->intTimeArray[device->intTotalCounter] = cpu_time_used;
    device->intTotalCounter++;
    return ret;
}

