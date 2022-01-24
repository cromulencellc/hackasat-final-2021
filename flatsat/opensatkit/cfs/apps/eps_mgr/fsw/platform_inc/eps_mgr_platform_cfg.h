/*
** Purpose: Define platform configurations for the Eps_mgr application
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

#ifndef _eps_mgr_platform_cfg_
#define _eps_mgr_platform_cfg_

/*
** Includes
*/

#include "eps_mgr_mission_cfg.h"
#include "eps_mgr_msgids.h"
#include "eps_mgr_perfids.h"

/******************************************************************************
** Application Macros
*/

#define  EPS_MGR_RUNLOOP_DELAY    150  /* Delay in milliseconds */
// #define  EPS_USE_SCHEDULER

#define  EPS_MGR_CMD_PIPE_DEPTH    10
#define  EPS_MGR_CMD_PIPE_NAME     "EPS_MGR_CMD_PIPE"

#define  EPS_SENSOR_PIPE_DEPTH   1
#define  EPS_SENSOR_PIPE_NAME    "EPS_SENSOR_PIPE"
#define  EPS_SENSOR_PIPE_TIMEOUT 0

/******************************************************************************
** EPS_CFG_Tbl Configuration
*/

#define  EPS_MGR_CFG_TBL_LOAD_FILE  "/cf/eps_mgr_cfg_tbl.json"
#define  EPS_MGR_CFG_TBL_DUMP_FILE  "/cf/eps_mgr_cfg_tbl_d.json"

#endif /* _eps_mgr_platform_cfg_ */
