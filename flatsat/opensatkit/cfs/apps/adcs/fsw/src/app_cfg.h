/*
** Purpose: Define configurations for the Adcs application.
**
** Notes:
**   1. These macros can only be built with the application and can't
**      have a platform scope because the same file name is used for
**      all applications following the object-based application design.
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/
#include "cfe.h"
#include "adcs_platform_cfg.h"
#include "osk_app_fw.h"

#if defined(_FLATSAT_SIM_FSW_) && defined(_FLATSAT_HARDWARE_FSW_)
#error "Only __FLATSAT_SIM_FSW_ or _FLATSAT_HARDWARE_FSW_ can be defined"
#endif

/******************************************************************************
** Adcs Application Macros
*/

#define  ADCS_MAJOR_VERSION      0
#define  ADCS_MINOR_VERSION      1
#define  ADCS_REVISION           0
#define  ADCS_MISSION_REV        0

/******************************************************************************
** Command Macros
**
*/

#define ADCS_CONF_TBL_CMD_FC       (CMDMGR_APP_START_FC + 0)
#define ADCS_SET_EXE_DELAY_CMD_FC  (CMDMGR_APP_START_FC + 1)
#define ADCS_HW_CMD_FC             (CMDMGR_APP_START_FC + 2)
#define ADCS_CTRL_MODE_FC          (CMDMGR_APP_START_FC + 3)
#define ADCS_CTRL_TGT_FC           (CMDMGR_APP_START_FC + 4)
#define ADCS_WHEEL_CMD_FC          (CMDMGR_APP_START_FC + 5)
#define ADCS_CTRL_PARAMS_CMD_FC    (CMDMGR_APP_START_FC + 6)
#define ADCS_FSS_CAL_CMD_FC        (CMDMGR_APP_START_FC + 11)
#define ADCS_UPDATE_FLAG_CMD_FC    (CMDMGR_APP_START_FC + 12)

// #define ADCS_MTR_STATE_CMD_FC      (CMDMGR_APP_START_FC + 5)
// #define ADCS_CAL_CMD_FC            (CMDMGR_APP_START_FC + 6)
// #define ADCS_SUN_YAW_CMD_FC        (CMDMGR_APP_START_FC + 7)
// #define ADCS_WHEEL_CMD_RAW_FC      (CMDMGR_APP_START_FC + 9)
// #define ADCS_ST_REG_CMD_FC         (CMDMGR_APP_START_FC + 10)

/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define ADCS_BASE_EID               (APP_FW_APP_BASE_EID +  0)
#define ADCS_CONF_TBL_BASE_EID      (APP_FW_APP_BASE_EID + 10)
#define ADCS_IMPL_BASE_EID          (APP_FW_APP_BASE_EID + 20)
#define ADCS_HW_BASE_EID            (APP_FW_APP_BASE_EID + 40)



/******************************************************************************
** Example Table Configurations
**
*/

#define ADCS_CONF_TBL_MAX_ENTRY_ID 32

#endif /* _app_cfg_ */
