/*
** Purpose: Define message IDs for the Eps_mgr application
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
#ifndef _eps_mgr_msgids_
#define _eps_mgr_msgids_

/*
** Command Message IDs
*/

#define  EPS_MGR_CMD_MID        0x1910
#define  EPS_MGR_SEND_HK_MID    0x1911
#define  EPS_EXECUTE_MID        0x19E2

/*
** Telemetry Message IDs
*/

#define  EPS_MGR_TLM_HK_MID     0x0910
#define  EPS_MGR_FSW_TLM_HK_MID 0x0911
#define  EPS_SIM_SENSOR_MID     0x0912
#define  EPS_SIM_STATUS_MID     0x0913


#endif /* _eps_mgr_msgids_ */
