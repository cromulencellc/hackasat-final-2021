/*
** Purpose: Define message IDs for the Sla_tlm application
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
#ifndef _sla_tlm_msgids_
#define _sla_tlm_msgids_

/*
** Command Message IDs
*/

#define  SLA_TLM_CMD_MID        0x1E08
#define  SLA_TLM_SEND_HK_MID    0x1E09

/*
** Telemetry Message IDs
*/

#define  SLA_TLM_TLM_HK_MID     0x0E08
#define  COMM_PING_RAW_MID      0x19E5
#define  COMM_PAYLOAD_TELEM_MID 0x19E6  
#define  COMM_SLA_TLM_MID  	    0x09F9 // message from comm app with key
#define  COMM_SLA_PING_MID      0x09FA // message from comm app with ping status

#endif /* _sla_tlm_msgids_ */
