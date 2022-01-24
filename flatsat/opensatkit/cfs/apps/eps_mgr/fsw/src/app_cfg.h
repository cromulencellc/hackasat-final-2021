/*
** Purpose: Define configurations for the Eps_mgr application.
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
#include "eps_mgr_platform_cfg.h"
#include "osk_app_fw.h"

/******************************************************************************
** Eps_mgr Application Macros
*/

#define  EPS_MGR_MAJOR_VERSION      0
#define  EPS_MGR_MINOR_VERSION      9
#define  EPS_MGR_REVISION           0
#define  EPS_MGR_MISSION_REV        0

/******************************************************************************
** Command Macros
**
*/

#define EPS_MGR_LOAD_TBL_CMD_FC                 (CMDMGR_APP_START_FC + 0)
#define EPS_MGR_DUMP_TBL_CMD_FC                 (CMDMGR_APP_START_FC + 1)
#define EPS_IMPL_SET_MODE_CMD_FC                (CMDMGR_APP_START_FC + 2)
#define EPS_IMPL_SET_COMPONENT_STATE_CMD_FC     (CMDMGR_APP_START_FC + 3)

/******************************************************************************
** Event Macros
** 
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define EPS_MGR_BASE_EID        (APP_FW_APP_BASE_EID +  0)
#define EPS_IMPL_BASE_EID       (APP_FW_APP_BASE_EID + 10)
#define EPS_CFG_BASE_EID        (APP_FW_APP_BASE_EID + 20)




/******************************************************************************
** Application Configurations
**
*/
#define EPS_IMPL_DEBUG 0
#define EPS_MODE_CNT    6
#define EPS_SWITCH_CNT  9
#define NUM_EPS 1
#define NUM_SOLAR_ARRAY 2

#endif /* _app_cfg_ */
