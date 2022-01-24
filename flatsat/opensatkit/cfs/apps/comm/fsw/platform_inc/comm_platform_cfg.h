/*
** Purpose: Define platform configurations for the Comm application
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _comm_platform_cfg_
#define _comm_platform_cfg_

/*
** Includes
*/

#include "comm_mission_cfg.h"
#include "comm_msgids.h"
#include "comm_perfids.h"

/******************************************************************************
** Application Macros
*/

#define  COMM_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  COMM_CMD_PIPE_DEPTH    10
#define  COMM_CMD_PIPE_NAME     "COMM_CMD_PIPE"

#endif /* _comm_platform_cfg_ */
