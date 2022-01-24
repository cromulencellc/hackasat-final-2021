### This file is a general .xdc for the Nexys Video Rev. A
### To use it in a project:
### - uncomment the lines corresponding to used pins
### - rename the used ports (in each line, after get_ports) according to the top level signal names in the project


## Clock Signal
set_property -dict { PACKAGE_PIN P15    IOSTANDARD LVCMOS33 } [get_ports { USER_CLOCK }]; #IO_L13P_T2_MRCC_34 Sch=sysclk
create_clock -period 20.833 -name USER_CLOCK [get_ports USER_CLOCK]
set_property CLOCK_DEDICATED_ROUTE BACKBONE [get_nets USER_CLOCK]

set_property -dict { PACKAGE_PIN H16    IOSTANDARD LVCMOS33 } [get_ports { ETH_REFCLK }];  # CONNECTOR B10 (input)
create_clock -period 20 -name ETH_REFCLK [get_ports ETH_REFCLK]
set_property CLOCK_DEDICATED_ROUTE TRUE [get_nets ETH_REFCLK]

create_clock -period 41.666 -name dco_clk {get_nets dcm_clk0}

# Leon-3 Payload UART
set_property -dict { PACKAGE_PIN R3   IOSTANDARD LVCMOS33 SLEW SLOW PULLTYPE PULLDOWN} [get_ports { LEON3_PAYLOAD_POWER_ENABLE }];    # CONNECTOR: D15
set_property -dict { PACKAGE_PIN P4   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_PAYLOAD_TXD }];    # CONNECTOR: D21
set_property -dict { PACKAGE_PIN L6   IOSTANDARD LVCMOS33 } [get_ports { LEON3_PAYLOAD_RXD }]; # CONNECTOR: C21

# Leon-3 EyasSat UART
set_property -dict { PACKAGE_PIN K16   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_EYASSAT_TXD }];    # CONNECTOR: A3
set_property -dict { PACKAGE_PIN J18   IOSTANDARD LVCMOS33 } [get_ports { LEON3_EYASSAT_RXD }];             # CONNECTOR: B3

## LEDs
set_property -dict { PACKAGE_PIN N4   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { STATUS_LED_MSP430 }];    # CONNECTOR: C23
set_property -dict { PACKAGE_PIN T1   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { STATUS_LED_RADIOLINK }]; # CONNECTOR: D23
#set_property -dict { PACKAGE_PIN M4   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_LED4 }];           # CONNECTOR: C24
set_property -dict { PACKAGE_PIN M4   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { BATTERY_GREEN_LED }];           # CONNECTOR: C24

#Unused -- Route to nowhere
set_property -dict { PACKAGE_PIN R1   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_LED5 }]; # CONNECTOR: D24
set_property -dict { PACKAGE_PIN M3   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_LED6 }]; # CONNECTOR: C25
set_property -dict { PACKAGE_PIN R2   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_LED7 }]; # CONNECTOR: D25

set_property -dict { PACKAGE_PIN M2   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { STATUS_LED_LEON_HEARTBEAT }]; # CONNECTOR: C26
set_property -dict { PACKAGE_PIN P2   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { STATUS_LED_EXTRA }]; # CONNECTOR: D26
#set_property -dict { PACKAGE_PIN K5   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { BATTERY_GREEN_LED }]; # CONNECTOR: C27
set_property -dict { PACKAGE_PIN K5   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_LED4 }]; # CONNECTOR: C27
set_property -dict { PACKAGE_PIN N2   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { BATTERY_YELLOW_LED }]; # CONNECTOR: D27
# End unused

## External Debug UART
# Original HAS1 Setup
#set_property -dict { PACKAGE_PIN B18   IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { EXTERNAL_DEBUG_TXD }]; # CONNECTOR: B21
#set_property -dict { PACKAGE_PIN A18   IOSTANDARD LVCMOS33 } [get_ports { EXTERNAL_DEBUG_RXD }];           # CONNECTOR: A21

# HAS2 Swap so that console uart doesn't turn on CDH
set_property -dict { PACKAGE_PIN C14   IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { EXTERNAL_DEBUG_TXD }]; # CONNECTOR: A24
set_property -dict { PACKAGE_PIN D14   IOSTANDARD LVCMOS33 } [get_ports { EXTERNAL_DEBUG_RXD }];           # CONNECTOR: B24

## External Console UART
# Original HAS1 Setup
#set_property -dict { PACKAGE_PIN C14 IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { EXTERNAL_CONSOLE_TXD }]; # CONNECTOR: A24
#set_property -dict { PACKAGE_PIN D14 IOSTANDARD LVCMOS33 } [get_ports { EXTERNAL_CONSOLE_RXD }];           # CONNECTOR: B24

# HAS2 Swap so that console uart doesn't turn on CDH
set_property -dict { PACKAGE_PIN B18 IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { EXTERNAL_CONSOLE_TXD }]; # CONNECTOR: B21
set_property -dict { PACKAGE_PIN A18 IOSTANDARD LVCMOS33 } [get_ports { EXTERNAL_CONSOLE_RXD }];           # CONNECTOR: A21

## Leon-3 Umbilical UART
set_property -dict { PACKAGE_PIN K3  IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { LEON3_UMBILICAL_TXD }]; # CONNECTOR: C30
set_property -dict { PACKAGE_PIN L1  IOSTANDARD LVCMOS33 } [get_ports { LEON3_UMBILICAL_RXD }]; # CONNECTOR: D30

## Leon-3 Umbilical UART - EXP RADIO
#set_property -dict { PACKAGE_PIN K3   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_EXP_RADIO_TXD }];    # CONNECTOR: C30
#set_property -dict { PACKAGE_PIN L1   IOSTANDARD LVCMOS33 } [get_ports { LEON3_EXP_RADIO_RXD }];             # CONNECTOR: D30

## Jumpers (Input)
set_property -dict { PACKAGE_PIN U7  IOSTANDARD LVCMOS33 PULLTYPE PULLUP} [get_ports { UMBILICAL_SELECT_JUMPER }]; # CONNECTOR: C5
set_property PULLUP TRUE [get_ports UMBILICAL_SELECT_JUMPER]
set_property -dict { PACKAGE_PIN V7  IOSTANDARD LVCMOS33 PULLTYPE PULLUP} [get_ports { DEBUG_UART_SELECT_JUMPER }]; # CONNECTOR: D4
set_property PULLUP TRUE [get_ports DEBUG_UART_SELECT_JUMPER]
set_property -dict { PACKAGE_PIN U8  IOSTANDARD LVCMOS33 PULLTYPE PULLUP} [get_ports { CONSOLE_UART_SELECT_JUMPER }]; # CONNECTOR: C4
set_property PULLUP TRUE [get_ports CONSOLE_UART_SELECT_JUMPER]

set_property -dict { PACKAGE_PIN V9  IOSTANDARD LVCMOS33 PULLTYPE PULLUP} [get_ports { FLASH_SELECT_JUMPER }]; # CONNECTOR: D3
set_property PULLUP TRUE [get_ports FLASH_SELECT_JUMPER]
set_property -dict { PACKAGE_PIN U9  IOSTANDARD LVCMOS33 PULLTYPE PULLUP} [get_ports { EYASSAT_BUS_BYPASS_JUMPER }]; # CONNECTOR: C3
set_property PULLUP TRUE [get_ports EYASSAT_BUS_BYPASS_JUMPER]


## Leon-3 GPIOs
#set_property -dict { PACKAGE_PIN U9  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_GPIO2 }]; # CONNECTOR: C3

#set_property -dict { PACKAGE_PIN E15  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_GPIO5 }]; # CONNECTOR: B19 #E15~IO_L11P_T1_SRCC_15=sw[3]
#set_property -dict { PACKAGE_PIN C17  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_GPIO6 }]; # CONNECTOR: A20 #C17~IO_L20N_T3_A19_15=sw[4]
#set_property -dict { PACKAGE_PIN C16  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_GPIO7 }]; # CONNECTOR: B20 #C16~IO_L20P_T3_A20_15=sw[5]
##set_property -dict { PACKAGE_PIN A18  IOSTANDARD LVCMOS33 } [get_ports { sw[6] }]; # CONNECTOR: A21 #A18~IO_L10N_T1_AD11N_15=sw[6]
##set_property -dict { PACKAGE_PIN B18  IOSTANDARD LVCMOS33 } [get_ports { sw[7] }]; # CONNECTOR: B21 #B18~IO_L10P_T1_AD11P_15=sw[7]



## Leon3 Debug UART
#set_property -dict { PACKAGE_PIN C15  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_DEBUG_TXD }]; # CONNECTOR: A22 #C15~IO_L12N_T1_MRCC_15=uart_rx_out
#set_property -dict { PACKAGE_PIN D15  IOSTANDARD LVCMOS33 } [get_ports { LEON3_DEBUG_RXD }]; # CONNECTOR: B22 #D15~IO_L12P_T1_MRCC_15=uart_tx_in

## Leon3 Debug JTAG
#set_property -dict { PACKAGE_PIN V6  IOSTANDARD LVCMOS33 } [get_ports { LEON3_DEBUG_JTAG_TCK }]; # CONNECTOR: D5
#set_property -dict { PACKAGE_PIN U6  IOSTANDARD LVCMOS33 } [get_ports { LEON3_DEBUG_JTAG_TMS }]; # CONNECTOR: C6
#set_property -dict { PACKAGE_PIN V5  IOSTANDARD LVCMOS33 } [get_ports { LEON3_DEBUG_JTAG_TDI }]; # CONNECTOR: D6
#set_property -dict { PACKAGE_PIN T8  IOSTANDARD LVCMOS33 } [get_ports { LEON3_DEBUG_JTAG_TDO }]; # CONNECTOR: C7

## Leon3 to Ethernet SPI0
set_property -dict { PACKAGE_PIN F13  IOSTANDARD LVCMOS33 } [get_ports { ETHSPI_MOSI }]; # CONNECTOR: A18
set_property -dict { PACKAGE_PIN F14  IOSTANDARD LVCMOS33 } [get_ports { ETHSPI_MISO }]; # CONNECTOR: B18
set_property -dict { PACKAGE_PIN E15  IOSTANDARD LVCMOS33 } [get_ports { ETHSPI_SCK }]; # CONNECTOR: B19
set_property -dict { PACKAGE_PIN E16  IOSTANDARD LVCMOS33 } [get_ports { ETHSPI_CSN }]; # CONNECTOR: A19

## Leon3 to Payload I2C
set_property -dict { PACKAGE_PIN N6  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { PAYLOAD_I2C_SCL }]; # CONNECTOR: C19
set_property -dict { PACKAGE_PIN P5  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { PAYLOAD_I2C_SDA }]; # CONNECTOR: D19

## SPI Config Flash
set_property -dict { PACKAGE_PIN V5   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { MASTER_CONFIG_MOSI }];   # CONNECTOR: D6
set_property -dict { PACKAGE_PIN V6   IOSTANDARD LVCMOS33 } [get_ports { MASTER_CONFIG_MISO }];            # CONNECTOR: D5
set_property -dict { PACKAGE_PIN T8   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { MASTER_CONFIG_SCK }];    # CONNECTOR: C7
set_property -dict { PACKAGE_PIN U6   IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { MASTER_CONFIG_CSN }];    # CONNECTOR: C6

set_property -dict { PACKAGE_PIN V4  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_GPIO3 }]; # CONNECTOR: D7   (WP# for Config Flash)
set_property -dict { PACKAGE_PIN R8  IOSTANDARD LVCMOS33 SLEW SLOW} [get_ports { LEON3_GPIO4 }]; # CONNECTOR: C8   (RESET# for Config Flash)


## Radio Flash Connection
set_property -dict { PACKAGE_PIN B13   IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { RADIO_SCK }];        # CONNECTOR: B27
set_property -dict { PACKAGE_PIN A16   IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { RADIO_NSS }];        # CONNECTOR: A26
set_property -dict { PACKAGE_PIN C12   IOSTANDARD LVCMOS33 } [get_ports { RADIO_MISO }];                 # CONNECTOR: B28
set_property -dict { PACKAGE_PIN B14   IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { RADIO_MOSI }];       # CONNECTOR: A27

set_property -dict { PACKAGE_PIN A14   IOSTANDARD LVCMOS33 } [get_ports { RADIO_DIO0 }];        # CONNECTOR: A29
set_property -dict { PACKAGE_PIN A13   IOSTANDARD LVCMOS33 } [get_ports { RADIO_DIO1 }];        # CONNECTOR: B29
set_property -dict { PACKAGE_PIN B12   IOSTANDARD LVCMOS33 } [get_ports { RADIO_DIO2 }];        # CONNECTOR: A28
set_property -dict { PACKAGE_PIN B11   IOSTANDARD LVCMOS33 } [get_ports { RADIO_DIO3 }];        # CONNECTOR: A30
set_property -dict { PACKAGE_PIN A11   IOSTANDARD LVCMOS33 } [get_ports { RADIO_DIO4 }];        # CONNECTOR: B30
set_property -dict { PACKAGE_PIN D12   IOSTANDARD LVCMOS33 } [get_ports { RADIO_DIO5 }];        # CONNECTOR: B25

set_property -dict { PACKAGE_PIN A15   IOSTANDARD LVCMOS33 SLEW SLOW } [get_ports { RADIO_RESET }];      # CONNECTOR: B26

## Microchip KSZ9896C Ethernet PHY
#set_property -dict {PACKAGE_PIN E15  IOSTANDARD LVCMOS33 SLEW FAST } [get_ports { ETH_MDC }];  # CONNECTOR B19 (output) MDC
#set_property -dict {PACKAGE_PIN A9  IOSTANDARD LVCMOS33 } [get_ports { ETH_MDIO }];  # CONNECTOR A18 (output) MDIO
set_property -dict {PACKAGE_PIN C16 IOSTANDARD LVCMOS33 } [get_ports { ETH_RSTN }];            # CONNECTOR B20 (output) ETH_RESET_N
set_property -dict {PACKAGE_PIN E18 IOSTANDARD LVCMOS33 } [get_ports { ETH_CRSDV }];           # CONNECTOR A12 (input)
set_property -dict {PACKAGE_PIN F15 IOSTANDARD LVCMOS33 } [get_ports { ETH_RXERR }];           # CONNECTOR B12 (input)
set_property -dict {PACKAGE_PIN D17 IOSTANDARD LVCMOS33 } [get_ports { ETH_RXD_0 }];           # CONNECTOR B14 (input)
set_property -dict {PACKAGE_PIN E17 IOSTANDARD LVCMOS33 } [get_ports { ETH_RXD_1 }];           # CONNECTOR B13 (input)
set_property -dict {PACKAGE_PIN H17 IOSTANDARD LVCMOS33 } [get_ports { ETH_TXEN }];            # CONNECTOR A8 (output)
set_property -dict {PACKAGE_PIN F18 IOSTANDARD LVCMOS33 SLEW FAST } [get_ports { ETH_TXD_0 }]; # CONNECTOR A11 (output)
set_property -dict {PACKAGE_PIN G18 IOSTANDARD LVCMOS33 SLEW FAST } [get_ports { ETH_TXD_1 }]; # CONNECTOR A10 (output)
#set_property -dict {PACKAGE_PIN H16 IOSTANDARD LVCMOS33 } [get_ports { ETH_REFCLK }];          # CONNECTOR B10 (input)



###set_property -dict { PACKAGE_PIN B8    IOSTANDARD LVCMOS33 } [get_ports { eth_intn }]; #IO_L12P_T1_MRCC_16 Sch=eth_intn


