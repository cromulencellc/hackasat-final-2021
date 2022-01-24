#ifndef _config_h_
#define _config_h_

#include "stdio.h"
#include "stdbool.h"
#include "stdint.h"

#define TRUE 1
#define FALSE 0


#define PDB_LOG_LEVEL 3
#define SERIAL_OUTPUT Serial2

/** Status Defintions
 * 
 */
#define MESSAGE_FORMAT_IS_CCSDS
#define CFE_SUCCESS 0
/**
**  This error code will be returned when a request such as ...SetMsgTime
**  is made on a packet that does not include a field for msg time.
**
*/
#define CFE_SB_WRONG_MSG_TYPE     ((int32_t)0xca00000d)

/******************************************************************************
** PDB Message Ids
*/

#define     PDB_CMD_MID            0x1E01
#define     PDB_HK_TLM_MID         0x0E01
#define     PDB_CHARGER_TLM_MID    0x0E02

/******************************************************************************
** PDB UDP Comms Definitions
*/
#ifndef IP_ADDRESS_SEND
#define IP_ADDRESS_SEND "192.168.3.1"
#endif
#ifndef IP_ADDRESS_SEND
#define IP_ADDRESS "192.168.3.64"
#endif
#define PORT_SEND 9999
#define PORT_RECV 9998
#define ETHERNET_MAC 0xDEADBEEFAA01
#define RECV_PKT_SIZE 256
#define OUTPUT_CHANNEL_COUNT 6

/******************************************************************************
** PDB Application Macros
*/

#define  PDB_MAJOR_VERSION      0
#define  PDB_MINOR_VERSION      1
#define  PDB_REVISION           0
#define  PDB_MISSION_REV        0

/******************************************************************************
** Command Manager (CMDMGR)
*/

#define CMDMGR_CMD_FUNC_TOTAL  15

/* Standard function definitions */

#define NOOP_CMD_FC      0  
#define RESET_CMD_FC     1
#define SET_CHANNEL_OUTPUT 2  /* First FC available for app */

#endif