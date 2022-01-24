/*
** Purpose: Define message IDs for the Adcs application
**
** Notes:
**   None
**
** License:
**   Written by David McComas, licensed under the copyleft GNU General
**   Public License (GPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _adcs_msgids_
#define _adcs_msgids_

/*
** Command Message IDs
*/

#define     ADCS_CMD_MID            0x19DF
#define     ADCS_SEND_HK_MID        0x19E0
#define     ADCS_EXECUTE_MID        0x19E1
#define     ADCS_RW_CMD_MID         0x19E2
#define     ADCS_SIM_SENSOR_DATA_MID 0x019E3

/*
** Telemetry Message IDs
*/

#define  ADCS_TLM_HK_MID            0x09E5
#define  ADCS_FSW_TLM_MID           0x09E6
#define  ADCS_SIM_ACTUATOR_DATA_MID 0x09E7
// #define  ADCS_CTRL_GAINS_TLM_MID    0x09E7
#define  ADCS_HW_IMU_TLM_MID        0x09E8
#define  ADCS_HW_MTR_TLM_MID        0x09E9
#define  ADCS_HW_RW_TLM_MID         0x09EA
#define  ADCS_HW_XADC_TLM_MID       0x09EB
#define  ADCS_HW_ST_TLM_MID         0x09EC
#define  ADCS_HW_FSS_TLM_MID        0x09ED

#endif /* _adcs_msgids_ */
