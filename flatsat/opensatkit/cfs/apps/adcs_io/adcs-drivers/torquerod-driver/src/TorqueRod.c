
#include "TorqueRod.h"


/******************************************************************************
************************  EXPORTED FUNCTIONS  *********************************
******************************************************************************/

/******************************************************************************
** Function: mtr_constructor
**
*/
void mtr_constructor(const char* device_name, MtrDevice* input_device) {
    snprintf(input_device->device_name, TORQUEROD_NAME_MAX_CHAR, "%s", device_name);
    input_device->active = false;
    input_device->reset = false;
    input_device->state = MTR_OFF;
}

/******************************************************************************
** Function: mtr_init
**
*/
int mtr_init(const char* chipname_p, unsigned int line_num_p, const char* chipname_n, unsigned int line_num_n, MtrDevice* device) {
    int ret = MTR_FAILURE;
    bool ret_p = false;
    bool ret_n = false;
    if (device != NULL) {
        char consumer[MAX_GPIO_DEVICE_NAME];
        snprintf(consumer, MAX_GPIO_DEVICE_NAME, "%s%s", device->device_name, GPIO_P_CONSUMER);
        ret_p = gpio_init_output(&device->gpio_p, chipname_p, line_num_p, consumer, false, NULL, NULL, 0);
        snprintf(consumer, MAX_GPIO_DEVICE_NAME, "%s%s", device->device_name, GPIO_N_CONSUMER);
        ret_n = gpio_init_output(&device->gpio_n, chipname_n, line_num_n, consumer, false, NULL, NULL, 0);
        if (ret_p && ret_n) {
            ret = MTR_SUCCESS;
            device->active = true;
            device->reset = false;
        } else {
            ADCS_IO_LOG_ERROR("Torque rod init failed");
        }
    }
    return ret;
}

/******************************************************************************
** Function: mtr_set_state
**
*/
int mtr_set_state(MtrDevice* device, MtrState state) {
    int ret = MTR_FAILURE;
    int ret_p, ret_n;
    if((device != NULL) && device->active) {
        switch (state) {
            case MTR_OFF:
                ret_p = gpio_write(&device->gpio_p, 0);
                ret_n = gpio_write(&device->gpio_n, 0);
                break;
            case MTR_TORQUE_P:
                ret_p = gpio_write(&device->gpio_p, 1);
                ret_n = gpio_write(&device->gpio_n, 0);
                break;
            case MTR_TORQUE_N:
                ret_p = gpio_write(&device->gpio_p, 0);
                ret_n = gpio_write(&device->gpio_n, 1);
                break;
            default:
                ret_p = gpio_write(&device->gpio_p, 0);
                ret_n = gpio_write(&device->gpio_n, 0);
        }
        ret = (ret_p & ret_n);
        if (ret == MTR_FAILURE) {
            device->reset = true;
            ADCS_IO_LOG_ERROR("Torque rod GPIO write failed");
        } else {
            device->state = state;
        }
    }
    return ret;
}

/******************************************************************************
** Function: mtr_reset
**
*/
int mtr_reset(MtrDevice* device) {
    int ret = MTR_FAILURE;
    if (device != NULL) {
        mtr_close(device);
        ret = mtr_init(device->gpio_p.chipname, device->gpio_p.line_offset, device->gpio_n.chipname, device->gpio_n.line_offset, device);
        if (ret == MTR_SUCCESS) {
            device->reset = false;
        }
    }
    return ret;
}

/******************************************************************************
** Function: mtr_close
**
*/
void mtr_close(MtrDevice* device) {
    if (device != NULL) {
        ADCS_IO_LOG_DEBUG("Close MTR %s\n", device->device_name);
        mtr_set_state(device, MTR_OFF);
    }
}

/******************************************************************************
************************  LOCAL FUNCTIONS  *********************************
******************************************************************************/
