#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <linux/i2c.h>
#include <linux/i2c-dev.h>
#include "i2c_device.h"

// Local function prototypes
int i2c_ioctl(I2CDevice *i2c_device, long io_type);

void i2c_init(I2CDevice* i2c_device, I2CPeripheralType_t peripheralType) {

    if (peripheralType == AXI_I2C) {
        i2c_device->max_read_size = I2C_MAX_READ_AXI;
    } else {
        i2c_device->max_read_size = I2C_MAX_BUFFER_SIZE;
    }
    i2c_device->max_write_size = I2C_MAX_BUFFER_SIZE;
    i2c_device->active = false;
}

void i2c_open(const char *i2c_device_name, uint8_t addr, I2CDevice *i2c_device)
{
    int fd;
    int device_name_len =  strlen(i2c_device_name);
    if (device_name_len > MAX_DEVICE_NAME_LEN) {
        ADCS_IO_LOG_ERROR("Device name too long, probably not real I2C device");
        return;
    }
    strncpy(i2c_device->name, i2c_device_name, strlen(i2c_device_name));
    /* Open i2c-bus devcice */
    if ((fd = open(i2c_device_name, O_RDWR)) == -1) {
        ADCS_IO_LOG_ERROR("Could not open I2C device %s, error no: %d", i2c_device_name, errno);
        i2c_device->fd = -1;
        return;
    }

    i2c_device->addr = addr;
    i2c_device->fd = fd;
    i2c_device->active = true;
    if (i2c_detect(i2c_device)) {
        i2c_device->active = true;
    } else {
        i2c_device->active = false;
        i2c_close(i2c_device);
    }

}

bool i2c_detect(I2CDevice *i2c_device) {
    unsigned char data;
    int i2cDetectAttemptCnt = 0;
    int status;
    bool ret;
    do {
        status = i2c_read(i2c_device, &data, 1);
        if (status < 1) {
            ADCS_IO_LOG_ERROR("Could not find I2C device at address 0x%02X", i2c_device->addr);
            ret = false;
        } else {
            ADCS_IO_LOG_INFO("I2C Device Detected at address 0x%02X", i2c_device->addr);
            ret = true;
            break;
        }
        i2cDetectAttemptCnt++;
    }
    while(i2cDetectAttemptCnt < I2C_DETECT_ATEMPTS);
    return ret;
}

void i2c_close(I2CDevice *i2c_device)
{
    close(i2c_device->fd);
    i2c_device->active = false;
}

// int i2c_ioctl(I2CDevice *i2c_device, long io_type) {
//     int status = ioctl(i2c_device->fd, I2C_SLAVE, i2c_device->addr);
//     if (status < 0) {
//         printf("ioctl error setting up communications with addr %02X\n", i2c_device->addr);
//     }
//     return status;
// }

int i2c_write(I2CDevice *i2c_device, uint8_t* buf, unsigned int len) {
    int status = -1;
    if (i2c_device != NULL && i2c_device->active) {
        status = ioctl(i2c_device->fd, I2C_SLAVE, i2c_device->addr);
        if (status < 0) {
            ADCS_IO_LOG_ERROR("ioctl error setting up communications with addr %02X, error no: %d", i2c_device->addr, errno);
        }
        status = write(i2c_device->fd, buf, len);
        if (status < 0) {
            ADCS_IO_LOG_ERROR("I2C %s device write error, error no: %d\n", i2c_device->name, errno);
        }
    
    }
    return status;
}

int i2c_read(I2CDevice *i2c_device, uint8_t* buf, unsigned int len) {
    int status = -1;
    if (i2c_device != NULL && i2c_device->active) {
        status = ioctl(i2c_device->fd, I2C_SLAVE, i2c_device->addr);
        if (status < 0) {
            ADCS_IO_LOG_ERROR("I2C %s ioctl error setting up communications with addr %02X, error no: %d", i2c_device->name, i2c_device->addr, errno);
        }
        status = read(i2c_device->fd, buf, len);
        if (status < 0) {
            ADCS_IO_LOG_ERROR("I2C %s device read error, error no: %d", i2c_device->name, errno);
        }
    }
    return status;
}

int i2c_combined_write_read(I2CDevice *i2c_device, uint8_t write_byte, uint8_t* buf, unsigned int len) {
    int status = -1;
    if (i2c_device != NULL && i2c_device->active) {
        struct i2c_msg rdwr_msgs[2] = {
            {  // Start address
            .addr = i2c_device->addr,
            .flags = 0, // write
            .len = 1,
            .buf = &write_byte
            },
            { // Read buffer
            .addr = i2c_device->addr,
            .flags = 1, // read
            .len = len,
            .buf = buf
            }
        };

        struct i2c_rdwr_ioctl_data msg_data = {
            .msgs = rdwr_msgs,     /* ptr to array of simple messages */
            .nmsgs = 2              /* number of messages to exchange */
        };

        status = ioctl(i2c_device->fd, I2C_RDWR, &msg_data);
        
        if (status < 0) {
            ADCS_IO_LOG_ERROR("I2C %s device repeated start read error, error no: %d", i2c_device->name, errno);
        } else {
            status = len;
        }
    }

    return status;
}




/******************************************************************************
** Function: i2c_min
**
*/
int i2c_min(int a, int b) {
    return ((a) < (b) ? (a) : (b));
}
