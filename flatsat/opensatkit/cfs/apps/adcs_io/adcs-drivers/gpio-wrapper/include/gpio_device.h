#ifndef _GPIO_DEVICE_H
#define _GPIO_DEVICE_H

#include <gpiod.h>
#include <stdio.h>
#include <unistd.h>
#include "adcs_io_logger.h"

#define MAX_GPIO_DEVICE_NAME 32
#define GPIO_SUCCESS 0
#define GPIO_FAILURE -1

typedef enum gpio_state_e {
    GPIO_LOW = 0,
    GPIO_HIGH = 1
} gpio_state_t;

typedef int (*gpio_event_callback)(int, const struct timespec *);

/* 
* Lifecycle struct to manage GPIO pin
*/

typedef struct gpio_device_s {
    bool active;
    unsigned int line_offset; /**< @brief [int] GPIO line offset on selected chip (e.g. 0, 1) */
    bool active_low;/**< @brief [bool] Indicates if line is active low (required for falling edge events) */
    int value; /**< @brief [int] Current value read or written to GPIO line */
    int flags; /**< @brief [int] GPIO line flags used by bias and drive flags */
    char chipname[MAX_GPIO_DEVICE_NAME]; /**< @brief [char*] GPIO device chip name (e.g. gpiochip1020) */
    char consumer[MAX_GPIO_DEVICE_NAME]; /**< @brief [char*] GPIO consumer strin gname (e.g. "gpio_consumer") */
    gpio_event_callback event_callback;
} gpio_device_t;


/*
* GPIO Function Prototypes
*/

/**
* @brief - Function to initialize gpio_device_t with passed in parameters
* @param device - pointer to gpio_device_t to initialize
* @param chipname - GPIO chipname from /sys/bus/gpio/devices
* @param line_offset - GPIO line offset to select for selected chip (0, 1, etc...)
* @param line_offset - GPIO consumer string
* @param active_low - Flag to indicate if GPIO line should be treated as active low
* @return true if GPIO line is available, false if not
*
* Uses libgpiod for GPIO control
*/
bool gpio_init(gpio_device_t *device, const char* chipname, int line_offset, const char* consumer, bool active_low);


/**
* @brief - Function to initialize gpio_device_t with passed in parameters
* @param device - pointer to gpio_device_t to initialize
* @param chipname - GPIO chipname from /sys/bus/gpio/devices
* @param line_offset - GPIO line offset to select for selected chip (0, 1, etc...)
* @param line_offset - GPIO consumer string
* @param active_low - Flag to indicate if GPIO line should be treated as active low
* @param bias_flags - Bias flags for gpio line (pull-up, pull-down, disable, as-is)
* @return true if GPIO line is available, false if not
*
* Uses libgpiod for GPIO control
*/
bool gpio_init_input(gpio_device_t *device, const char* chipname, int line_offset, const char* consumer, bool active_low, const char* bias_flags);

/**
* @brief - Function to initialize gpio_device_t with passed in parameters
* @param device - pointer to gpio_device_t to initialize
* @param chipname - GPIO chipname from /sys/bus/gpio/devices
* @param line_offset - GPIO line offset to select for selected chip (0, 1, etc...)
* @param line_offset - GPIO consumer string
* @param active_low - Flag to indicate if GPIO line should be treated as active low
* @param bias_flags - Bias flags for gpio line (pull-up, pull-down, disable)
* @param drive_flags - Drive flags for gpio line (open-drain, open-source)
* @return true if GPIO line is available, false if not
*
* Uses libgpiod for GPIO control
*/
bool gpio_init_output(gpio_device_t *device, const char* chipname, int line_offset, const char* consumer, bool active_low, const char* bias_flags, const char* drive_flags, int initial_value);

/**
* @brief - Function to initialize gpio_device_t with passed in parameters
* @param device - pointer to gpio_device_t to initialize
* @param callback - function to call on gpio event
*
*/
void gpio_register_event_callback(gpio_device_t *device, gpio_event_callback callback);

/**
* @brief - Function to set value of GPIO Output device
* @param chipname - GPIO chipname from /sys/bus/gpio/devices
* @param line_offset - GPIO line offset to select for selected chip (0, 1, etc...)
* @return true if GPIO line is available, false if not
*
* Uses libgpiod for GPIO control
*/
bool gpio_check(const char* chipname, int line_offset);

/**
* @brief - Function to set value of GPIO Output device
* @param device - pointer to gpio_device_t for this gpio_open
* @param value - GPIO value GPIO_LOW (0V) or GPIO_HIGH (3.3V)
* @return GPIO_SUCESS if write was success, -1 if open fails
*
* Uses libgpiod for GPIO control
*/
int gpio_write(gpio_device_t *device, int value);

/**
* @brief - Function to set value of GPIO Output device
* @param device - pointer to gpio_device_t for this gpio_open
* @return value for GPIO read (0 or 1), -1 for error
*
* Uses libgpiod for GPIO control
*/
int gpio_read(gpio_device_t *device);

/**
* @brief - Function to monitor for rising edge events. This enters a poll loop
* @param device - pointer to gpio_device_t for this gpio_open
* @param timeoutSeconds - double number for timeouts between loop iterations
* @return GPIO_SUCCESS if no errors, GPIO_FAILURE if error
*
* Uses libgpiod for GPIO control
*/
int gpio_mon_rising_edge(gpio_device_t *device, double timeoutSeconds);

/**
* @brief - Function to monitor for falling edge events. This enters a poll loop
* @param device - pointer to gpio_device_t for this gpio_open
* @param timeoutSeconds - double number for timeouts between loop iterations
* @return GPIO_SUCCESS if no errors, GPIO_FAILURE if error
*
* Uses libgpiod for GPIO control
*/
int gpio_mon_falling_edge(gpio_device_t *device, double timeoutSeconds);


#endif