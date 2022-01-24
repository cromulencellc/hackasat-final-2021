#ifndef _I2C_DEVICE_H
#define _I2C_DEVICE_H

#include <stdint.h>
#include <stdbool.h>
#include "adcs_io_logger.h"

#define I2C_MAX_BUFFER_SIZE 8192
#define I2C_MAX_READ_AXI 255
#define MAX_DEVICE_NAME_LEN 20
#define I2C_DETECT_ATEMPTS 5

typedef enum {
    AXI_I2C,
    LINUX_I2C
} I2CPeripheralType_t;

typedef struct i2c_device_t {
    bool active;
    int fd;
    uint8_t addr;
    unsigned int max_read_size;
    unsigned int max_write_size;
    char name[MAX_DEVICE_NAME_LEN];
} I2CDevice;


/*
* I2C Function Prototypes
*/
void i2c_init(I2CDevice* i2c_device, I2CPeripheralType_t peripheralType);
void i2c_open(const char *i2c_device_name, uint8_t addr, I2CDevice *i2c_device);
bool i2c_detect(I2CDevice *i2c_device);
void i2c_close(I2CDevice *i2c_device);
int i2c_write(I2CDevice *i2c_device, uint8_t* buf, unsigned int len);
int i2c_read(I2CDevice *i2c_device, uint8_t* buf, unsigned int len);
int i2c_combined_write_read(I2CDevice *i2c_device, uint8_t reg_addr, uint8_t* buf, unsigned int len);
int i2c_min(int a, int b);

#endif