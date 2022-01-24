
#include <signal.h>
#include "Cubewheel.h"

#define WAIT_STEP 10

void print_tlm_data(FILE* stream, cubewheel_device_t* wheel, bool header);
void wheel_duty_test(cubewheel_device_t *wheel, FILE* file);
void wheel_speed_test(cubewheel_device_t *wheel, FILE* file);
void tlm_rate_test(cubewheel_device_t *wheel, FILE* file);
static int keepRunning = 1;

void intHandler(int value) {
    ADCS_IO_LOG_WARN("Interrupt recieved, quit gracefully\n");
    keepRunning = 0;
}

int main(int argc, char* argv[]) {

    signal(SIGINT, intHandler);
    FILE *outputFile = fopen("wheel_tlme.log", "w");
    cubewheel_device_t wheel;
    ADCS_IO_LOG_INFO("Cubewheel Contstructor\n");
    cubewheel_constructor(&wheel, 0);
    // ADCS_IO_LOG_INFO("Cubewheel Setup GPIO\n");
    // cubewheel_gpio_init(&wheel, "gpiochip4", 0);
    ADCS_IO_LOG_INFO("Cubewheel Init GPIO and I2C\n");
    cubewheel_init(&wheel, "/dev/i2c-0", CUBEWHEEL_I2CADDR, CUBEWHEEL_DEFAULT_I2C_TYPE, "gpiochip4", 0);
    // ADCS_IO_LOG_INFO("Cubewheel Init /w I2C\n");
    // cubewheel_i2c_begin(&wheel);
    // exit(0);

    // float wheelRpm = 0;
    bool tcStatus;
    bool tlmStatus;
    cubewheel_telecommand_t *tc;
    if(wheel.data.active) {
        tlmStatus = cubewheel_get_periodic_tlm(&wheel);
        print_tlm_data(outputFile, &wheel, true);
        ADCS_IO_LOG_INFO("TLM Status %d\n", tlmStatus);
        // while(keepRunning && tlmStatus) {
        //     ADCS_IO_LOG_INFO("Started Tests\n\n");
        //     wheel_speed_test(&wheel, outputFile);
        //     // sleep(10);
        //     // wheel_duty_test(&wheel, outputFile);
        //     // tlm_rate_test(&wheel, outputFile);
        //     keepRunning = false;
        // }
        tc = cubewheel_tc_set_control_mode(WHEEL_MODE_USE_SPEED_CONTROL);
        tcStatus = cubewheel_send_tc(&wheel, tc);
        tc = cubewheel_tc_set_reference_speed((float) 500.0);
        tcStatus = cubewheel_send_tc(&wheel, tc);
        msleep(WAIT_STEP * 1000);
        tlmStatus = cubewheel_get_periodic_tlm(&wheel);
        print_tlm_data(outputFile, &wheel, true);
    } else {
        ADCS_IO_LOG_ERROR("Cubewheel driver init failed");
    }
    ADCS_IO_LOG_INFO("Close Cubewheel Driver\n");
    cubewheel_close(&wheel);
    fclose(outputFile);
}

void print_tlm_data(FILE* stream, cubewheel_device_t* wheel, bool header) {
    if (header) {
        fprintf(stream, "Runtime (s:msec), Wheel Speed, Wheel Reference Speed, Wheel Current, Wheel Duty Cycle, Wheel Speed Backup, Motor Control Mode, Backup Control Mode, Motor Switch State, Hall Sensor Switch State, Encoder Sensor Switch State, Error Flag\n");
        printf("Runtime (s:msec), Wheel Speed, Wheel Reference Speed, Wheel Current, Wheel Duty Cycle, Wheel Speed Backup, Motor Control Mode, Backup Control Mode, Motor Switch State, Hall Sensor Switch State, Encoder Sensor Switch State, Error Flag\n");
    }
    fprintf(stream, "%u:%u, %7.3f, %7.3f, %7.3f, %d, %7.3f, %u, %u, %u, %u, %u, %u\n",
                wheel->data.wheelStatus.runtimeSeconds,
                wheel->data.wheelStatus.runtimeMiliseconds,
                raw_value_to_rpm(wheel->data.wheelData.wheelSpeed),
                raw_value_to_rpm(wheel->data.wheelData.wheelReferenceSpeed),
                raw_value_to_current(wheel->data.wheelData.wheelCurrent),
                wheel->data.wheelDataAdditional.wheelDuty,
                raw_value_to_rpm(wheel->data.wheelDataAdditional.wheelSpeedBackup),
                wheel->data.wheelStatus.motorControlMode,
                wheel->data.wheelStatus.backupModeState,
                wheel->data.wheelStatus.motorSwitchState,
                wheel->data.wheelStatus.hallSensorSwitchState,
                wheel->data.wheelStatus.encoderSensorSwitchState,
                wheel->data.wheelStatus.errorFlag);
    printf("%u:%u, %7.3f, %7.3f, %7.3f, %d, %7.3f, %u, %u, %u, %u, %u, %u\n",
                wheel->data.wheelStatus.runtimeSeconds,
                wheel->data.wheelStatus.runtimeMiliseconds,
                raw_value_to_rpm(wheel->data.wheelData.wheelSpeed),
                raw_value_to_rpm(wheel->data.wheelData.wheelReferenceSpeed),
                raw_value_to_current(wheel->data.wheelData.wheelCurrent),
                wheel->data.wheelDataAdditional.wheelDuty,
                raw_value_to_rpm(wheel->data.wheelDataAdditional.wheelSpeedBackup),
                wheel->data.wheelStatus.motorControlMode,
                wheel->data.wheelStatus.backupModeState,
                wheel->data.wheelStatus.motorSwitchState,
                wheel->data.wheelStatus.hallSensorSwitchState,
                wheel->data.wheelStatus.encoderSensorSwitchState,
                wheel->data.wheelStatus.errorFlag);
}

void wheel_speed_test(cubewheel_device_t *wheel, FILE* file) {
    float wheelRpm = 0;
    bool tcStatus;
    bool tlmStatus;
    uint32_t timeElapsed = 0;
    cubewheel_telecommand_t *tc;
    tc = cubewheel_tc_set_control_mode(WHEEL_MODE_USE_SPEED_CONTROL);
    tcStatus = cubewheel_send_tc(wheel, tc);
    tlmStatus = cubewheel_get_periodic_tlm(wheel);
    print_tlm_data(file, wheel, false);
    // sleep(5);
    int i;
    // ADCS_IO_LOG_INFO("Spin Wheel to 5000 -> -5000 -> 0 rpm\n");
    for (i=0; i<30; i=i+5) {
        if (!keepRunning)
            break;
        while(timeElapsed < WAIT_STEP) {
            if (!keepRunning)
                break;
            ADCS_IO_LOG_INFO("GET TLM PERIODIC");
            tlmStatus = cubewheel_get_periodic_tlm(wheel);
            print_tlm_data(file, wheel, false);
            ADCS_IO_LOG_INFO("TC SET REFERENCE SPEED -> %f rpm", (float) i);
            tc = cubewheel_tc_set_reference_speed((float) i);
            tcStatus = cubewheel_send_tc(wheel, tc);
            msleep(1000);
            timeElapsed++;
        }
        timeElapsed = 0;
    }
    // for (i=5000; i>-5000; i=i-500) {
    //     if (!keepRunning)
    //         break;
    //     tc = cubewheel_tc_set_reference_speed((float) i);
    //     tcStatus = cubewheel_send_tc(wheel, tc);
    //     while(timeElapsed < WAIT_STEP) {
    //         tlmStatus = cubewheel_get_periodic_tlm(wheel);
    //         print_tlm_data(file, wheel, false);
    //         sleep(1);
    //         timeElapsed++;
    //     }
    //     timeElapsed = 0;
    // }
    // for (i=-5000; i<0; i=i+500) {
    //     if (!keepRunning)
    //         break;
    //     tc = cubewheel_tc_set_reference_speed((float) i);
    //     tcStatus = cubewheel_send_tc(wheel, tc);
    //     while(timeElapsed < WAIT_STEP) {
    //         tlmStatus = cubewheel_get_periodic_tlm(wheel);
    //         print_tlm_data(file, wheel, false);
    //         sleep(1);
    //         timeElapsed++;
    //     }
    //     timeElapsed = 0;
    // }
    tc = cubewheel_tc_set_reference_speed(0.0);
    tcStatus = cubewheel_send_tc(wheel, tc);
}

void wheel_duty_test(cubewheel_device_t *wheel, FILE* file) {
    float wheelRpm = 0;
    bool tcStatus;
    bool tlmStatus;
    uint32_t timeElapsed = 0;
    cubewheel_telecommand_t *tc;
    tc = cubewheel_tc_set_control_mode(WHEEL_MODE_DUTY_CYCLE_CONTROL);
    tcStatus = cubewheel_send_tc(wheel, tc);
    tlmStatus = cubewheel_get_periodic_tlm(wheel);
    print_tlm_data(file, wheel, false);
    sleep(5);
    int i;
    ADCS_IO_LOG_INFO("Spin Wheel to 95 -> -95 -> 0 rpm\n");
    for (i=5; i<25; i=i+10) {
        if (!keepRunning)
            break;
        tc = cubewheel_tc_set_duty_cycle((float) i);
        tcStatus = cubewheel_send_tc(wheel, tc);
        while(timeElapsed < WAIT_STEP) {
            tlmStatus = cubewheel_get_periodic_tlm(wheel);
            print_tlm_data(file, wheel, false);
            sleep(1);
            timeElapsed++;
        }
        timeElapsed = 0;
    }
    for (i=25; i>-25; i=i-10) {
        if (!keepRunning)
            break;
        tc = cubewheel_tc_set_duty_cycle((float) i);
        tcStatus = cubewheel_send_tc(wheel, tc);
        while(timeElapsed < WAIT_STEP) {
            tlmStatus = cubewheel_get_periodic_tlm(wheel);
            print_tlm_data(file, wheel, false);
            sleep(1);
            timeElapsed++;
        }
        timeElapsed = 0;
    }
    for (i=-25; i<0; i=i+10) {
        if (!keepRunning)
            break;
        tc = cubewheel_tc_set_duty_cycle((float) i);
        tcStatus = cubewheel_send_tc(wheel, tc);
        while(timeElapsed < WAIT_STEP) {
            tlmStatus = cubewheel_get_periodic_tlm(wheel);
            print_tlm_data(file, wheel, false);
            sleep(1);
            timeElapsed++;
        }
        timeElapsed = 0;
    }
    tc = cubewheel_tc_set_duty_cycle(0);
    tcStatus = cubewheel_send_tc(wheel, tc);
    sleep(20);
}

void tlm_rate_test(cubewheel_device_t *wheel, FILE* file) {
    bool tcStatus;
    bool tlmStatus = true;
    uint32_t timeElapsed = 0;
    int sleep_msec = 1000;
    int i =0;
    int delta = 100;
    while(tlmStatus) {
        for (i=0; i<5; i++) {
            if (!tlmStatus) {
                tlmStatus = false;
                ADCS_IO_LOG_WARN("Tlm Status Failed. Rate test complete\nFinal Time: %d msec", sleep_msec);
                break;
            }

            tlmStatus = cubewheel_get_periodic_tlm(wheel);
            print_tlm_data(file, wheel, false);
            msleep(sleep_msec);
        }
        sleep_msec = sleep_msec - delta;
        ADCS_IO_LOG_INFO("New TLM Rate %d msec\n", sleep_msec);
        if (sleep_msec < 0) {
                break;
        }
    }
    ADCS_IO_LOG_INFO("TLM Rate Test Complete\n");
}