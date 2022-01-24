
#include <signal.h>
#include <pthread.h>
#include "time.h"

#include "BNO08x_Driver.h"

#define NUM_SENSORS 4

void updateLoop(double updatePeriod);
void* interuptIoThread(void *arg);

BNO08x_Device device;
BNO08x_Sensor_Data *sensorDataList;
const int numSensors = NUM_SENSORS;
unsigned int loopIterations = 0;

pthread_t tid;
static int keepRunning = 1;
bool exitReady = false;

void intHandler(int value) {
    keepRunning = 0;
}

int main(int argc, char* argv[]) {

    signal(SIGINT, intHandler);

    int updatePeriod = 100; // Milliseconds
    if (argc==2) {
        updatePeriod = atoi(argv[1]);
    }

    ADCS_IO_LOG_INFO("Starting Main Program /w Update Rate: %d msec\n", updatePeriod);

    // Allocate array space for sensor data

   
    // sh2_SensorId_t sensorsIds[1] = {SH2_GYROSCOPE_CALIBRATED};
    int numSensors = NUM_SENSORS;
    sensorDataList = malloc(numSensors*sizeof(BNO08x_Sensor_Data));
    memset(sensorDataList, 0, numSensors*sizeof(BNO08x_Sensor_Data));
    
    // Setup IMU
    ADCS_IO_LOG_INFO("bno08x_constructor");
    bno08x_constructor(&device);
    bno08x_set_reset_line("gpiochip3", 1);
    bno08x_set_interrupt_line("gpiochip3", 0);
    // ADCS_IO_LOG_INFO("bno08x_set_interrupt_line");
    // bno08x_set_interrupt_line("gpiochip3", 0);
    // ADCS_IO_LOG_INFO("bno08x_set_reset_line");
    // bno08x_set_reset_line("gpiochip3", 0);
    // ADCS_IO_LOG_INFO("bno08x_set_boot_line");
    // bno08x_set_boot_line("gpiochip3", 2);

    ADCS_IO_LOG_INFO("bno08x_init");
    bno08x_init("/dev/i2c-1", BNO08X_I2CADDR_DEFAULT, AXI_I2C);

    // Reset prior to tests
    ADCS_IO_LOG_INFO("bno08x_hardware_reset");
    bno08x_hardware_reset();

    if (!bno08x_open()) {
        exit(1);
    }

    // int err = pthread_create(&tid, NULL, &interuptIoThread, NULL);
    // if (err != 0) {
    //     ADCS_IO_LOG_INFO("Could not create thread\n");
    // } else {
    //     ADCS_IO_LOG_INFO("Thread started successfully\n");
    // }
    // dsleep(2);
    // bno08x_disable_interrupt_updates();
    // pthread_join(tid, NULL);

    unsigned int updatePad = 0; // usec
    bno08x_init_sensor_data(&sensorDataList[0], SH2_GYROSCOPE_CALIBRATED, sizeof(sh2_Gyroscope_t), updatePeriod/2);
    bno08x_init_sensor_data(&sensorDataList[1], SH2_ACCELEROMETER, sizeof(sh2_Accelerometer_t), updatePeriod/2);
    bno08x_init_sensor_data(&sensorDataList[2], SH2_MAGNETIC_FIELD_CALIBRATED, sizeof(sh2_MagneticField_t), updatePeriod/2);
    bno08x_init_sensor_data(&sensorDataList[3], SH2_ROTATION_VECTOR, sizeof(sh2_RotationVectorWAcc_t), updatePeriod/2);

    // Init all reports
    bno08x_enable_dynamic_calibration(true, true, true);
    bno08x_setup_reports(sensorDataList, numSensors);


    // bno08x_run_simple_calibration(BNO08X_DEFAULT_UPDATE_INTERVAL_US, &calResults);
    // ADCS_IO_LOG_INFO("Calibration Results: %d\n\n", calResults);
    updateLoop((double) updatePeriod/1e3);
    ADCS_IO_LOG_INFO("Sensor Loop Iterations: %u", loopIterations);
    ADCS_IO_LOG_INFO("Sensor Callback Cnt: %d", device.sensorCallbackCnt);
    for (int i=0; i<numSensors; i++) {
        ADCS_IO_LOG_INFO("Sensor %s, Revieved Cnt %u, Missed Cnt %u", bno08x_sensor_id_to_string(device.sensorDataList[i].sensorId), device.sensorDataList[i].sensorRecvCnt, device.sensorDataList[i].sensorSequenceMissedCnt);
    }


    // ADCS_IO_LOG_INFO("Test HW Reset\n");
    // bno08x_hardware_reset();
    // bno08x_init("/dev/i2c-0", BNO08X_I2CADDR_DEFAULT, AXI_I2C);
    // if(bno08x_open())
    //     ADCS_IO_LOG_INFO("BNO085 Opened after reset\n");
    
    // Init all reports
    // bno08x_setup_reports(sensorDataList, numSensors);
    
    // Start interupt handler thread

    // bno08x_disable_interrupt_updates();
    // pthread_join(tid, NULL);
    // ADCS_IO_LOG_INFO("Timout Counter: %d\n", device.intTimeoutCounter);
    // ADCS_IO_LOG_INFO("Total Interrupts: %d\n", device.intTotalCounter);
    // ADCS_IO_LOG_INFO("Sensor Interrupts: %d\n", device.intCounter);
    // for (int i=0; i<numSensors; i++) {
    //     ADCS_IO_LOG_INFO("Sensor ID %d, Revieved Cnt %d, Missed Cnt %d\n", device.sensorDataList[i].sensorId, device.sensorDataList[i].sensorRecvCnt, device.sensorDataList[i].sensorSequenceMissedCnt);
    // }
    // double mean = gsl_stats_mean(device.intTimeArray, 1, device.intTotalCounter);
    // double max = gsl_stats_max(device.intTimeArray, 1, device.intTotalCounter);
    // double min = gsl_stats_min(device.intTimeArray, 1, device.intTotalCounter);
    // double std = gsl_stats_sd(device.intTimeArray, 1, device.intTotalCounter);
    // ADCS_IO_LOG_INFO("Mean: %f, Std: %f, Min: %f, Max: %f\n", mean, std, min, max);
    // updateLoop(updatePeriod);
    bno08x_close();
    free(sensorDataList);
}

void updateLoop(double updatePeriod) {
        // Start update loop
    struct timespec start, end;
    double time_taken;
    uint32_t sensorUpdateCounts[NUM_SENSORS];
    memset(&sensorUpdateCounts, 0, sizeof(sensorUpdateCounts));
    sh2_SensorValue_t *sensorData;
    uint64_t timestampLast = 0;
    int idx = 0;
    bool status;
    double timeDiff;
    int counter = 0;
    keepRunning = true;
    while(keepRunning) {
        clock_gettime(CLOCK_MONOTONIC, &start);
        loopIterations++;
        if (loopIterations > 1000) {
            break;
        }
        ADCS_IO_LOG_INFO("Get updated data from IMU");
        status = bno08x_update();
        for (idx= 0; idx<numSensors; idx++) {
            ADCS_IO_LOG_INFO("Check for sensor update on sensorIdx: %d", idx);
            if (device.sensorDataList[idx].sensorRecvCnt > sensorUpdateCounts[idx]) {
                counter++;
                sensorData = &device.sensorDataList[idx].sensorValue;
                switch (sensorData->sensorId) {
                    case SH2_ACCELEROMETER:
                        ADCS_IO_LOG_INFO("Accelerometer: Time: %llu, x: %12.10e, y: %12.10e, z: %12.10e", sensorData->timestamp, sensorData->un.accelerometer.x, sensorData->un.accelerometer.y, sensorData->un.accelerometer.z);
                        break;
                    case SH2_LINEAR_ACCELERATION:
                        ADCS_IO_LOG_INFO("Linear Accelerometer: Time: %llu, x: %12.10e, y: %12.10e, z: %12.10e", sensorData->timestamp, sensorData->un.linearAcceleration.x, sensorData->un.linearAcceleration.y, sensorData->un.linearAcceleration.z);
                        break;
                    case SH2_GYROSCOPE_CALIBRATED:
                        ADCS_IO_LOG_INFO("Gyro: Time: %llu, x: %12.10e, y: %12.10e, z: %12.10e", sensorData->timestamp, sensorData->un.gyroscope.x, sensorData->un.gyroscope.y, sensorData->un.gyroscope.z);
                        break;
                    case SH2_ROTATION_VECTOR:
                        ADCS_IO_LOG_INFO("Rotation Vector: Time: %llu, qs: %12.10e, qx: %12.10e, qy: %12.10e, qz: %12.10e, Accuracy: %12.10e", sensorData->timestamp, sensorData->un.rotationVector.real, sensorData->un.rotationVector.i, sensorData->un.rotationVector.j, sensorData->un.rotationVector.k, sensorData->un.rotationVector.accuracy);
                        break;
                    case SH2_MAGNETIC_FIELD_CALIBRATED:
                        ADCS_IO_LOG_INFO("Magnetic Field: Time: %llu, x: %12.10e, y: %12.10e, z: %12.10e", sensorData->timestamp, sensorData->un.magneticField.x, sensorData->un.magneticField.y, sensorData->un.magneticField.z);
                        break;
                    default:
                        ADCS_IO_LOG_INFO("Unknown report. ID: %d\n", sensorData->sensorId);
                    
                }
                ADCS_IO_LOG_INFO("Sequence Number: %d, Recv Cnt: %d, Cnt Missed %d", sensorData->sequence, device.sensorDataList[idx].sensorRecvCnt, device.sensorDataList[idx].sensorSequenceMissedCnt);            
                ADCS_IO_LOG_INFO("Sensor Reported Accuracy: %s", bno08x_sensor_status_to_string(idx));

                sensorUpdateCounts[idx]++;

                if (sensorUpdateCounts[idx] != device.sensorDataList[idx].sensorRecvCnt) {
                    ADCS_IO_LOG_WARN("Missed update for %s", bno08x_sensor_id_to_string(sensorData->sensorId));
                    sensorUpdateCounts[idx] = device.sensorDataList[idx].sensorRecvCnt;
                }
            }
        }

        clock_gettime(CLOCK_MONOTONIC, &end);
        time_taken = (end.tv_sec - start.tv_sec) * 1e9; 
        time_taken = (time_taken + (end.tv_nsec - start.tv_nsec)) * 1e-9;
        double time_rem = updatePeriod  - time_taken;
        // ADCS_IO_LOG_INFO("Sleep %f secs until next iter", time_rem);
        dsleep(time_rem);
    }
}

void* interuptIoThread(void *arg) {
    unsigned long i = 0;
    pthread_t id = pthread_self();
    bno08x_enable_interrupt_updates(1.0);
    ADCS_IO_LOG_INFO("Thread exiting\n");
    exitReady = true;
    return NULL;
}
