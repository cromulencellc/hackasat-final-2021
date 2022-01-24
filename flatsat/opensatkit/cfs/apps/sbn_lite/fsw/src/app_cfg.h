/*
** Purpose: Define configurations for the SBN-lite app. 
**
*/

#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "cfe_platform_cfg.h"
#include "sbn_lite_platform_cfg.h"
#include "osk_app_fw.h"

/******************************************************************************
** SBN-lite Application Macros
*/

/*
** Versions:
**
*/

#define  SBN_LITE_MAJOR_VER     1
#define  SBN_LITE_MINOR_VER     0
#define  SBN_LITE_LOCAL_REV     0


#define  SBN_LITE_CMD_PIPE_DEPTH    10
#define  SBN_LITE_CMD_PIPE_NAME     "SBN_LITE_CMD_PIPE"
#define  SBN_LITE_RUNLOOP_MSG_READ  400



/******************************************************************************
** Command Macros
*/

#define SBN_LITE_PKT_TBL_LOAD_CMD_FC       (CMDMGR_APP_START_FC +  0)
#define SBN_LITE_PKT_TBL_DUMP_CMD_FC       (CMDMGR_APP_START_FC +  1)

#define SBN_LITE_ADD_PKT_CMD_FC            (CMDMGR_APP_START_FC +  2)
#define SBN_LITE_ENABLE_OUTPUT_CMD_FC      (CMDMGR_APP_START_FC +  3)
#define SBN_LITE_REMOVE_ALL_PKTS_CMD_FC    (CMDMGR_APP_START_FC +  4)
#define SBN_LITE_REMOVE_PKT_CMD_FC         (CMDMGR_APP_START_FC +  5)
#define SBN_LITE_SEND_PKT_TBL_TLM_CMD_FC   (CMDMGR_APP_START_FC +  6)
#define SBN_LITE_UPDATE_FILTER_CMD_FC      (CMDMGR_APP_START_FC +  7)

// #define KIT_TO_SEND_DATA_TYPES_CMD_FC    (CMDMGR_APP_START_FC +  8)
#define SBN_LITE_SET_RUN_LOOP_DELAY_CMD_FC (CMDMGR_APP_START_FC +  9)
// #define KIT_TO_TEST_FILTER_CMD_FC        (CMDMGR_APP_START_FC + 10)

/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define SBN_LITE_APP_BASE_EID  (APP_FW_APP_BASE_EID +   0)
#define SBNTBL_BASE_EID      (APP_FW_APP_BASE_EID + 100)
#define SBNMGR_BASE_EID      (APP_FW_APP_BASE_EID + 200)

/*
** One event ID is used for all initialization debug messages. Uncomment one of
** the KIT_TO_INIT_EVS_TYPE definitions. Set it to INFORMATION if you want to
** see the events during initialization. This is opposite to what you'd expect 
** because INFORMATION messages are enabled by default when an app is loaded.
*/

#define SBN_LITE_INIT_DEBUG_EID 999
#define SBN_LITE_INIT_EVS_TYPE CFE_EVS_DEBUG
//#define KIT_TO_INIT_EVS_TYPE CFE_EVS_INFORMATION


/******************************************************************************
** pktmgr.h Configurations
*/

/*
** Pipe configurations
*/

#define SBNMGR_PIPE_DEPTH  256
#define SBNMGR_PIPE_NAME   "SBN_LITE_PKT_PIPE"

/*
** Statistics
*/

#define SBNMGR_STATS_STARTUP_INIT_MS   20000  /* ms after app initialized to start stats computations   */
#define SBNMGR_STATS_RECONFIG_INIT_MS   5000  /* ms after a reconfiguration to start stats computations */

#define SBNMGR_COMPUTE_STATS_INTERVAL_MS SBN_LITE_MAX_RUN_LOOP_DELAY_MS /* ms between stats updates       */

#define SBNMGR_RECV_BUFF_LEN  2048

#endif /* _app_cfg_ */
