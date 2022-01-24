#ifndef _pdb_pins_h_
#define _pdb_pins_h_

#include <Arduino.h>

/**
 * USER LEDs
*/
#define LED1 0
#define LED2 1
#define LED3 2
#define LED4 3

/**
 * User Switches
*/
#define SW1 4
#define SW2 5
#define SW3 6
#define SW4 9

/**
 * Debug UART
*/
#define UART_DEBUG_RX 7
#define UART_DEBUG_TX 8

/**
 * System SPI
*/
// #define SPI_CS PIN_SP
// #define SPI_MOSI PIN_A11
// #define SPI_MISO PIN_A12
// #define SPI_CLK PIN_A13

/**
 * I2C
*/
// #define I2C_TEMP_SCL PIN_A19
// #define I2C_TEMP_SDA PIN_A18
// #define I2C_PWR_SCL PIN_A16
// #define I2C_PWR_SDA PIN_A17

/**
 * Analog Muxes
*/
#define AMUX_S1 28
#define AMUX_S2 29
#define AMUX_S3 30
#define AMUX_EN 31



/**
 * Analog Pins
*/
#define ANALOG_CH_VOLT_IN A10
#define ANALOG_CH_CUR_IN A11
#define ANALOG_MISC_IN A12
// #define ANALOG_RSVD_IN

/**
 * Voltage Output Channel Enable GPIO
*/
#define OUPTUT_CH1_EN 33
#define OUPTUT_CH2_EN 34
#define OUPTUT_CH3_EN 35
#define OUPTUT_CH4_EN 36
#define OUPTUT_CH5_EN 37
#define OUPTUT_CH6_EN 38

/**
 * Voltage Regulator Power Good
*/
#define VR_1_PWR_GOOD 40
#define VR_2_PWR_GOOD 39

/**
 * Temperature Alert
*/
#define TEMP_ALARM 15

/**
 * USB-PD
*/
#define USB_PD_ATTACH 21
#define USB_PD_ALERT 20


/**
 * Fuel Gauge
*/
#define FG_ALERT_1 22
#define FG_ALERT_2 23

#endif