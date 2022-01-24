/*
** Purpose: Define message IDs for the Comm application
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
#ifndef _comm_msgids_
#define _comm_msgids_

/*
** Command Message IDs
*/

#define  COMM_CMD_MID        0x19F6
#define  COMM_SEND_HK_MID    0x19F7

/*
** Telemetry Message IDs
*/

#define  COMM_SLA_RAW_MID 	 0x019E4
#define  COMM_TLM_HK_MID     0x09F6
#define  COMM_MOD_TLM_MID    0x09F7
#define  COMM_DEMOD_TLM_MID  0x09F8
#define  COMM_SLA_TLM_MID  	 0x09F9
#define  COMM_SLA_PING_MID   0x09FA

#endif /* _comm_msgids_ */
