#ifndef _TORQUE_ROD_H
#define _TORQUE_ROD_H

#include <stdio.h>
#include <unistd.h>
#include "gpio_device.h"
#include "adcs_io_logger.h"


#define TORQUEROD_NAME_MAX_CHAR 20
#define MTR_SUCCESS 1
#define MTR_FAILURE -1
#define GPIO_P_CONSUMER "_P"
#define GPIO_N_CONSUMER "_N"

typedef enum  {
    MTR_OFF,
    MTR_TORQUE_P,
    MTR_TORQUE_N
} MtrState;

typedef struct torque_rod_device_t {
    bool active;
    bool reset;
    MtrState state;
    gpio_device_t gpio_p;
    gpio_device_t gpio_n;
    char device_name[TORQUEROD_NAME_MAX_CHAR];
} MtrDevice;


/*
* Exported Functions
*/

/**
* @brief - function to construct magnetic torque rod with device name
* @param device_name GPIO chipname from /sys/bus/gpio/devices
* @param input_device - pointer to MtrDevice
*/
void mtr_constructor(const char* device_name, MtrDevice* input_device);

/**
* @brief - function to init GPIO devices for magnetic torque rod device
* @param chipname_p GPIO chipname from /sys/bus/gpio/devices for positive magnetic torque
* @param line_num_p GPIO line number for on chipname_p for posititive magnetic torque
* @param chipname_n GPIO chipname from /sys/bus/gpio/devices for negative magnetic torque
* @param line_num_n GPIO line number for on chipname_n for negative magnetic torque
* @param device - pointer to MtrDevice
* @return MTR_SUCCESS if init was success, -1 if init fails
*/
int mtr_init(const char* chipname_p, unsigned int line_num_p, const char* chipname_n, unsigned int line_num_n, MtrDevice* device);

/**
* @brief - function to close all GPIO devices for magnetic torque rod device
* @param device - pointer to MtrDevice
*/
void mtr_close(MtrDevice* device);

/**
* @brief - function to set state magnetic torque rod device
* @param device - pointer to MtrDevice
* @param state MtrState for magnetic torque rod from /sys/bus/gpio/devices for positive magnetic torque
* @return MTR_SUCCESS if init was success, -1 if init fails
*/
int mtr_set_state(MtrDevice* devicem, MtrState state);

/**
* @brief - function to reset magnetic torque rod device
* @param device - pointer to MtrDevice for reset
* @return MTR_SUCCESS if init was success, -1 if reset fails
*/
int mtr_reset(MtrDevice* device);

#endif