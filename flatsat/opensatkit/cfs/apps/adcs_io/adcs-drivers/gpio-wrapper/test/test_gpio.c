#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include "gpio_device.h"

static int eventCount = 0;
static gpio_device_t device[5];


void gpio_cb(int event_type, struct timespec *event_time) {
    ADCS_IO_LOG_INFO("Event Found. Type: %d Time %ld:%ld\n", event_type, event_time->tv_sec, event_time->tv_nsec);
    eventCount++;
    // if (eventCount > 4) {
    //     return GPIOD_CTXLESS_EVENT_CB_RET_STOP;
    // }
}


int main(int argc, char **argv)
{
    const char *chipname = "gpiochip0"; // AXI GPIO
    unsigned int line_num = 0; // GPIO Pin #13
    int channels = 5;
    int i;
    int ret;
    ADCS_IO_LOG_INFO("--------------OUTPUT TEST STARTED-------------");
    for (int i=0; i<channels; i++) {
        if(gpio_init_output(&device[i], chipname, i, "consumer", false, NULL, NULL, 1))
            device[i].active = true;
        ADCS_IO_LOG_INFO("Setup %s, line %d as output", chipname, i);
    }
    
    ADCS_IO_LOG_INFO("Cycle through LEDs on board");
    for (i=0; i<channels; i++) {
        ADCS_IO_LOG_INFO("GPIO Write Chip %s line %d, value %d\n", device[i].chipname, device[i].line_offset, 1);
        ret = gpio_write(&device[i], 0);
        sleep(1);
        ret = gpio_write(&device[i], 1);
        sleep(1);
    }

    ADCS_IO_LOG_INFO("--------------OUTPUT TEST COMPLETE-------------");
    ADCS_IO_LOG_INFO("--------------INPUT TEST STARTED-------------");
   
    const char *inputchipname = "gpiochip0"; // AXI GPIO
    channels=4;
    memset(device, 0, channels*sizeof(gpio_device_t));
    for (i=0; i<channels; i++) {
        if(gpio_init_input(&device[i], inputchipname, i, "consumer", false, NULL))
            device[i].active = true;
        ADCS_IO_LOG_INFO("Setup %s, line %d as input", chipname, i);
    }

    for (i=0; i<channels; i++) {
        ADCS_IO_LOG_INFO("GPIO Read Chip %s line %d, value %d\n", device[i].chipname, device[i].line_offset, gpio_read(&device[i]));
        sleep(1);
    }

    ADCS_IO_LOG_INFO("Monitor GPIO for falling edge ---- SWITCH 1 turning off (gpiochip2, line 0)\n");
    gpio_register_event_callback(&device[0], (gpio_event_callback) &gpio_cb);
    ret = gpio_mon_falling_edge(&device[0], 10);
    ADCS_IO_LOG_INFO("--------------INPUT TEST COMPLETE-------------");

}