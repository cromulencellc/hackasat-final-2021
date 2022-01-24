#include <stdio.h>
#include <string.h>
#include "gpio_device.h"

int set_bias_flags(const char *option);
static int set_drive_flags(const char *option);
int gpio_callback_wrapper(int event_type, unsigned int gpio_line_offset, const struct timespec * event_timestamp, void * data);

/******************************************************************************
** Function: gpio_init
**
*/
bool gpio_init(gpio_device_t *device, const char* chipname, int line_offset, const char* consumer, bool active_low) {
    device->line_offset = line_offset;
    device->active_low = active_low;
    device->active = false;
    device->flags = 0;
    snprintf(device->chipname, MAX_GPIO_DEVICE_NAME, "%s", chipname);
    snprintf(device->consumer, MAX_GPIO_DEVICE_NAME, "%s", consumer);
    return gpio_check(chipname, line_offset);
}

/******************************************************************************
** Function: gpio_init
**
*/
bool gpio_init_input(gpio_device_t *device, const char* chipname, int line_offset, const char* consumer, bool active_low, const char* bias_flags) {
    bool ret = false;
    ADCS_IO_LOG_DEBUG("gpio_init_input(). Chipname: %s, Line Offset: %d, Consumer: %s, Active Low: %d, bias flags: %d", 
        chipname, line_offset, consumer, active_low, bias_flags);
    if (gpio_init(device, chipname, line_offset, consumer, active_low)) {
        device->flags |= set_bias_flags(bias_flags);
        if (device->flags < 0) {
            return GPIO_FAILURE;
        }
        device->active = true;
        device->value = gpio_read(device);
        if (device->value >= 0) {
            ret = true;
        }
    }
    return ret;
}


bool gpio_init_output(gpio_device_t *device, const char* chipname, int line_offset, const char* consumer, bool active_low, const char* bias_flags, const char* drive_flags, int initial_value)
{
    bool ret = false;
    ADCS_IO_LOG_DEBUG("gpio_init_output(). Chipname: %s, Line Offset: %d, Consumer: %s, Active Low: %d, bias flags: %d, drive flags %d, initial value %d", 
        chipname, line_offset, consumer, active_low, bias_flags, drive_flags, initial_value);
    if (gpio_init(device, chipname, line_offset, consumer, active_low)) {
        device->flags |= set_bias_flags(bias_flags);
        if (device->flags < 0) {
            ADCS_IO_LOG_ERROR("GPIO BIAS Flags Could not be set");
            return false;
        }
        device->flags |= set_drive_flags(drive_flags);
        if (device->flags < 0) {
            ADCS_IO_LOG_ERROR("GPIO Drive Flags Could not be set");
            return false;
        }
        device->active = true;
        int status = gpio_write(device, initial_value);
        if (status == GPIO_SUCCESS) {
            ret = true;
        }
    }
    return ret;
}

/******************************************************************************
** Function: gpio_check
**
*/
bool gpio_check(const char* chipname, int line) {
    bool ret = false;
    struct gpiod_line *line_check = gpiod_line_get(chipname, line);
    if (line_check != NULL) {
        gpiod_line_close_chip(line_check);
        ret = true;
    }
    return ret;
}

/******************************************************************************
** Function: gpio_write
**
*/
int gpio_write(gpio_device_t *device, int value) {
    int ret = 0;
    if (device != NULL && device->active) {
        device->value = value;
        ADCS_IO_LOG_DEBUG("GPIO Write. Chipname %s, Line %d, Value %d", device->chipname, device->line_offset, value);
        ret = gpiod_ctxless_set_value(device->chipname, device->line_offset, device->value, device->active_low, device->consumer, NULL, NULL);
        if (ret < 0) {
            ADCS_IO_LOG_ERROR("Could not write to GPIO Device");
        }

    } else {
        ADCS_IO_LOG_ERROR("Device NULL or not active");
    }
    return ret;
}

/******************************************************************************
** Function: gpio_register_event_callback
**
*/
void gpio_register_event_callback(gpio_device_t *device, gpio_event_callback callback) {
    if (callback != NULL)
        device->event_callback = callback;
}

/******************************************************************************
** Function: gpio_read
**
*/
int gpio_read(gpio_device_t *device) {
    int ret = 0;
    if (device != NULL && device->active) {
        ret = gpiod_ctxless_get_value_ext(device->chipname, device->line_offset, device->active_low, device->consumer, device->flags);
        if (ret < 0) {
            ADCS_IO_LOG_ERROR("Could not read GPIO Device");
        }
        ADCS_IO_LOG_DEBUG("GPIO Read. Chipname %s, Line %d, Value %d", device->chipname, device->line_offset, ret);
        device->value = ret;
    } else {
        ADCS_IO_LOG_ERROR("Device NULL or not active");
    }
    return ret;
}

/******************************************************************************
** Function: gpio_mon_rising_edge
*/
int gpio_mon_rising_edge(gpio_device_t *device, double timeoutSeconds) {
    int ret = GPIO_FAILURE;
    if (device != NULL && device->active) {
        struct timespec wait_ts;
        wait_ts.tv_sec = (int) timeoutSeconds;
        wait_ts.tv_nsec = (int)((timeoutSeconds - (double) wait_ts.tv_sec)*1E9);
        ret = gpiod_ctxless_event_monitor_ext(device->chipname, GPIOD_CTXLESS_EVENT_RISING_EDGE, device->line_offset, device->active_low, device->consumer, &wait_ts, NULL, &gpio_callback_wrapper, (void*) device, device->flags);
    } else {
        ADCS_IO_LOG_ERROR("Device NULL or not active");
    }

    return ret;
}

/******************************************************************************
** Function: gpio_mon_falling_edge
*/
int gpio_mon_falling_edge(gpio_device_t *device, double timeoutSeconds) {
    int ret = GPIO_FAILURE;
    if (device != NULL && device->active && device->active_low) {
        struct timespec wait_ts;
        wait_ts.tv_sec = (int) timeoutSeconds;
        wait_ts.tv_nsec = (int)((timeoutSeconds - (double) wait_ts.tv_sec)*1E9);
        ret = gpiod_ctxless_event_monitor_ext(device->chipname, GPIOD_CTXLESS_EVENT_FALLING_EDGE, device->line_offset, device->active_low, device->consumer, &wait_ts, NULL, &gpio_callback_wrapper, (void*) device, device->flags);
    } else {
        ADCS_IO_LOG_ERROR("Device NULL or not active");
    }
    return ret;
}


/******************************************************************************
** Function: bias_flags
*/
int set_bias_flags(const char *option)
{
    if (option == NULL)
        return 0;
    if (strcmp(option, "pull-down") == 0)
        return GPIOD_CTXLESS_FLAG_BIAS_PULL_DOWN;
    if (strcmp(option, "pull-up") == 0)
        return GPIOD_CTXLESS_FLAG_BIAS_PULL_UP;
    if (strcmp(option, "disable") == 0)
        return GPIOD_CTXLESS_FLAG_BIAS_DISABLE;
    return 0;
}

/******************************************************************************
** Function: drive_flags
*/
static int set_drive_flags(const char *option)
{
    if (option == NULL)
        return 0;
    if (strcmp(option, "open-drain") == 0)
        return GPIOD_CTXLESS_FLAG_OPEN_DRAIN;
    if (strcmp(option, "open-source") == 0)
        return GPIOD_CTXLESS_FLAG_OPEN_SOURCE;
    return 0;
}

int gpio_callback_wrapper(int event_type, unsigned int gpio_line_offset, const struct timespec * event_timestamp, void * data) {
    int ret;
    gpio_device_t *device = (gpio_device_t*) data;
    if (!device->active) {
        ret =  GPIOD_CTXLESS_EVENT_CB_RET_STOP;
    } else {
        ADCS_IO_LOG_DEBUG("GPIO Event Detected. Chipname %s, Line Num: %d, Event Type: %d, Timestamp (%ld:%ld)", device->chipname, gpio_line_offset, event_type, event_timestamp->tv_sec, event_timestamp->tv_nsec);
        if (device->event_callback != NULL) {
            ret = device->event_callback(event_type, event_timestamp);
        }
    }
    return ret;
}

