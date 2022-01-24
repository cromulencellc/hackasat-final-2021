
#include <signal.h>
#include <stdio.h>
#include "BNO08x_Driver.h"

#define NUM_SENSORS 2

BNO08x_Device device;
BNO08x_Sensor_Data *sensorDataList;
const int numSensors = 3;

static int keepRunning = 1;
bool exitReady = false;

void intHandler(int value) {
    keepRunning = 0;
}

void calLoop(int updatePeriod) {
    // Start update loop
    uint32_t sensorUpdateCounts[2] = {0, 0};
    sh2_SensorValue_t *sensorData;
    int idx = 0;
    bool status;
    char ch;
    keepRunning = true;
    while(keepRunning) {
        status = bno08x_update();
        for (idx= 0; idx<numSensors; idx++) {
            // ADCS_IO_LOG_INFO("Check for sensor update on sensorIdx: %d\n", idx);
            if (device.sensorDataList[idx].sensorRecvCnt > sensorUpdateCounts[idx]) {
                sensorData = &device.sensorDataList[idx].sensorValue;
                switch (sensorData->sensorId) {
                    case SH2_ROTATION_VECTOR:
                        // ADCS_IO_LOG_INFO("Rotation Vector: Time: %llu, qs: %f, qx: %f, qy: %f, qz: %f\n", sensorData->timestamp, sensorData->un.rotationVector.real, sensorData->un.rotationVector.i, sensorData->un.rotationVector.j, sensorData->un.rotationVector.k);
                        ADCS_IO_LOG_INFO("Rotation Vector /w Accuracy: Time: %llu, qs: %f, qx: %f, qy: %f, qz: %f, Accuracy: %f (rad)\n", sensorData->timestamp, sensorData->un.rotationVector.real, sensorData->un.rotationVector.i, sensorData->un.rotationVector.j, sensorData->un.rotationVector.k, sensorData->un.rotationVector.accuracy);
                        break;
                    case SH2_MAGNETIC_FIELD_CALIBRATED:
                        ADCS_IO_LOG_INFO("Magnetic Field: Time: %llu, x: %f, y: %f, z: %f\n", sensorData->timestamp, sensorData->un.magneticField.x, sensorData->un.magneticField.y, sensorData->un.magneticField.z);
                        break;
                    case SH2_GYROSCOPE_CALIBRATED:
                        ADCS_IO_LOG_INFO("Gyro: Time: %llu, x: %f, y: %f, z: %f\n", sensorData->timestamp, sensorData->un.gyroscope.x, sensorData->un.gyroscope.y, sensorData->un.gyroscope.z);
                        break;
                    default:
                        ADCS_IO_LOG_INFO("Unknown report. ID: %d\n", sensorData->sensorId);
                    
                }
                ADCS_IO_LOG_INFO("Sequence Number: %d, Recv Cnt: %d, Recv Cnt Last %d\n", sensorData->sequence, device.sensorDataList[idx].sensorRecvCnt, sensorUpdateCounts[idx]);            
                ADCS_IO_LOG_INFO("Sensor Reported Accuracy: %s\n", bno08x_sensor_status_to_string(idx));
                if (sensorUpdateCounts[idx] != device.sensorDataList[idx].sensorRecvCnt-1) {
                    ADCS_IO_LOG_INFO("Missed report for sensorIdx: %d\n", idx);
                }
                sensorUpdateCounts[idx] = device.sensorDataList[idx].sensorRecvCnt;
                ADCS_IO_LOG_INFO("\n\n");
            }
        }
        // ADCS_IO_LOG_INFO("Sleep until next update\n\n\n");
        msleep(updatePeriod);
    }
}


int main(int argc, char* argv[]) {

    signal(SIGINT, intHandler);

    int updatePeriod = 1; // Milliseconds
    ADCS_IO_LOG_INFO("Starting Main Program /w Update Rate: %d msec\n", updatePeriod);

    // Allocate array space for sensor data
   
    sensorDataList = malloc(numSensors*sizeof(BNO08x_Sensor_Data));
    
    // Setup IMU
    bno08x_constructor(&device);
    bno08x_set_reset_line("gpiochip0", 0);
    bno08x_set_interrupt_line("gpiochip0", 1);

    bno08x_init("/dev/i2c-0", BNO08X_I2CADDR_DEFAULT, AXI_I2C);

    // Reset prior to tests
    bno08x_hardware_reset();

    if (!bno08x_open()) {
        exit(1);
    }


    bno08x_init_sensor_data(&sensorDataList[0], SH2_ROTATION_VECTOR, sizeof(sh2_RotationVectorWAcc_t), BNO08X_DEFAULT_UPDATE_INTERVAL_US);
    bno08x_init_sensor_data(&sensorDataList[1], SH2_GYROSCOPE_CALIBRATED, sizeof(sh2_Gyroscope_t), 20000);
    bno08x_init_sensor_data(&sensorDataList[2], SH2_MAGNETIC_FIELD_CALIBRATED, sizeof(sh2_MagneticField_t), 20000);


    // Init all reports
    ADCS_IO_LOG_INFO("Enable Gyro Dynamic Calibration\n");
    bno08x_enable_dynamic_calibration(true, true, true);
    // ADCS_IO_LOG_INFO("Calibration Results: %d\n\n", calResults);
    bno08x_setup_reports(sensorDataList, numSensors);
    ADCS_IO_LOG_INFO("Starting Sensor Calibration\n");
    ADCS_IO_LOG_INFO("Perform acceleromerter calibration moves, then gyroscope calibration hold, then magnetometer calibration moves\n");
    ADCS_IO_LOG_INFO("Magnetometer and rotation vector status should go from UNRELAIBLE, eventually to HIGH\n");
    ADCS_IO_LOG_INFO("Press 'Ctrl-C' when sensor status is at correct state and all calibration moves are complete\n");
    ADCS_IO_LOG_INFO("Press any key to continue\n");
    getchar();
    calLoop(updatePeriod);

    // ADCS_IO_LOG_INFO("Sensor Callback Cnt: %d\n", device.sensorCallbackCnt);
    // for (int i=0; i<numSensors; i++) {
    //     ADCS_IO_LOG_INFO("Sensor ID %d, Revieved Cnt %d, Missed Cnt %d\n", device.sensorDataList[i].sensorId, device.sensorDataList[i].sensorRecvCnt, device.sensorDataList[i].sensorSequenceMissedCnt);
    // }

    
    bno08x_save_dynamic_calibration();
    bno08x_enable_dynamic_calibration(false, false, false);
    bno08x_close();

    free(sensorDataList);
}



