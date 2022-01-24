/*
** Purpose: Define platform configurations for the Adcs application
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

#ifndef _adcs_platform_cfg_
#define _adcs_platform_cfg_

/*
** Includes
*/

#include "adcs_mission_cfg.h"
#include "adcs_msgids.h"
#include "adcs_perfids.h"

/******************************************************************************
** Application Macros
*/
#define  ADCS_APP_RUNLOOP_DELAY       150 // Default cycle delay in msec
// #define  ADCS_USE_SCHEDULER // Included this line if you want to run using KIT_SCHED scheduler table

#define  ADCS_EXECUTE_MSG_CYCLES_MIN   1
#define  ADCS_EXECUTE_MSG_CYCLES_MAX  20
#define  ADCS_EXECUTE_MSG_CYCLES_DEF   1

#define  ADCS_EXECUTE_CYCLE_DELAY_MIN   10
#define  ADCS_EXECUTE_CYCLE_DELAY_MAX  250
#define  ADCS_EXECUTE_CYCLE_DELAY_DEF  200

#define  ADCS_SB_PIPE_DEPTH    50
#define  ADCS_SB_PIPE_NAME     "ADCS_SB_PIPE"
#define  ADCS_SB_PIPE_TIMEOUT  500

#define  ADCS_SB_EXECUTE_MSG_LIM  10

#define  ADCS_SENSOR_PIPE_DEPTH   1
#define  ADCS_SENSOR_PIPE_NAME    "ADCS_SENSOR_PIPE"
#define  ADCS_SENSOR_PIPE_TIMEOUT 0
/******************************************************************************
** Example Object Macros
*/

#define  ADCS_CONF_TBL_DEF_LOAD_FILE  "/cf/adcs_ctrl_tbl.json"
#define  ADCS_CONF_TBL_DEF_DUMP_FILE  "/cf/adcs_ctrl_tbl_d.json"

#endif /* _adcs_platform_cfg_ */
