/*
** Purpose: Define application configurations for the MQTT App
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**   2. These macros can only be build with the application and can't
**      have a platform scope because the same app_cfg.h file name is used for
**      all applications following the object-based application design.
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/
#ifndef _app_cfg_
#define _app_cfg_

/*
** Includes
*/

#include "mqtt_platform_cfg.h"
#include "osk_app_fw.h"

/******************************************************************************
** Application Macros
*/

/*
** Versions:
**
** 1.0.0 - Initial refactoring of open source FM
*/

#define  MQTT_MAJOR_VER      1
#define  MQTT_MINOR_VER      0

/******************************************************************************
**  INI File
*/

#define  MQTT_INI_MAX_STRINGS 20

/******************************************************************************
** Init File declarations create:
**
**  typedef enum {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigEnum;
**    
**  typedef struct {
**     CMD_PIPE_DEPTH,
**     CMD_PIPE_NAME
**  } INITBL_ConfigStruct;
**
**   const char *GetConfigStr(value);
**   ConfigEnum GetConfigVal(const char *str);
**
** XX(name,type)
*/

#define CFG_APP_CFE_NAME        APP_CFE_NAME
#define CFG_APP_RUNLOOP_DELAY   APP_RUNLOOP_DELAY

#define CFG_APP_MAIN_PERF_ID    APP_MAIN_PERF_ID
#define CFG_CHILD_TASK_PERF_ID  CHILD_TASK_PERF_ID

#define CFG_CMD_MID             CMD_MID
#define CFG_SEND_HK_MID         SEND_HK_MID

#define CFG_HK_TLM_MID          HK_TLM_MID
#define CFG_STR32_TLM_MID       STR32_TLM_MID

#define CFG_CMD_PIPE_DEPTH      CMD_PIPE_DEPTH
#define CFG_CMD_PIPE_NAME       CMD_PIPE_NAME

#define CFG_SERVER_PORT         SERVER_PORT
#define CFG_SERVER_ADDRESS      SERVER_ADDRESS
#define CFG_SERVER_USERNAME     SERVER_USERNAME
#define CFG_SERVER_PASSWORD     SERVER_PASSWORD
      
#define CFG_CLIENT_RECV_NAME                 CLIENT_RECV_NAME
#define CFG_CLIENT_RECV_KEEPALIVE            CLIENT_RECV_KEEPALIVE
#define CFG_CLIENT_YIELD_INTERVAL            CLIENT_YIELD_INTERVAL_MS

#define CFG_CLIENT_PUB_NAME                CLIENT_PUB_NAME
#define CFG_CLIENT_PUB_KEEPALIVE           CLIENT_PUB_KEEPALIVE
#define CFG_CLIENT_PUB_TIMEOUT_INTERVAL    CLIENT_PUB_TIMEOUT_INTERVAL_MS

// #define MQ2SB_CFG_TBL_CFE_NAME        MQ2SB_TBL_CFE_NAME
#define MQ2SB_CFG_TBL_DEF_FILENAME    MQ2SB_TBL_DEF_FILENAME
// #define MQ2SB_CFG_TBL_ERR_CODE        MQ2SB_TBL_ERR_CODE

// #define SB2MQ_CFG_TBL_CFE_NAME        SB2MQ_TBL_CFE_NAME
#define SB2MQ_CFG_TBL_DEF_FILENAME    SB2MQ_TBL_DEF_FILENAME
// #define SB2MQ_CFG_TBL_ERR_CODE        SB2MQ_TBL_ERR_CODE
#define SB2MQ_CFG_PIPE_DEPTH          SB2MQ_PIPE_DEPTH
#define SB2MQ_CFG_PIPE_NAME           SB2MQ_PIPE_NAME
#define SB2MQ_CFG_PKT_TLM_MID         SB2MQ_PKT_TLM_MID

#define CFG_CHILD_NAME          CHILD_NAME
#define CFG_CHILD_STACK_SIZE    CHILD_STACK_SIZE
#define CFG_CHILD_PRIORITY      CHILD_PRIORITY
#define CFG_CHILD_TASK_DELAY    CHILD_TASK_DELAY

/*
** Pipe configurations
*/



#define APP_CONFIG(XX) \
   XX(APP_CFE_NAME,char*) \
   XX(APP_RUNLOOP_DELAY,uint32) \
   XX(APP_MAIN_PERF_ID,uint32) \
   XX(CHILD_TASK_PERF_ID,uint32) \
   XX(CMD_MID,uint32) \
   XX(SEND_HK_MID,uint32) \
   XX(HK_TLM_MID,uint32) \
   XX(STR32_TLM_MID,uint32) \
   XX(CMD_PIPE_DEPTH,uint32) \
   XX(CMD_PIPE_NAME,char*) \
   XX(SERVER_PORT,uint32) \
   XX(SERVER_ADDRESS,char*) \
   XX(SERVER_USERNAME,char*) \
   XX(SERVER_PASSWORD,char*) \
   XX(CLIENT_RECV_NAME,char*) \
   XX(CLIENT_RECV_KEEPALIVE,uint32) \
   XX(CLIENT_YIELD_INTERVAL_MS,uint32) \
   XX(CLIENT_PUB_NAME,char*) \
   XX(CLIENT_PUB_KEEPALIVE,uint32) \
   XX(CLIENT_PUB_TIMEOUT_INTERVAL_MS,uint32) \
   XX(MQ2SB_TBL_DEF_FILENAME,char*) \
   XX(SB2MQ_TBL_DEF_FILENAME,char*) \
   XX(SB2MQ_PIPE_DEPTH,uint32) \
   XX(SB2MQ_PIPE_NAME,char*) \
   XX(SB2MQ_PKT_TLM_MID,uint32) \
   XX(CHILD_NAME,char*) \
   XX(CHILD_STACK_SIZE,uint32) \
   XX(CHILD_PRIORITY,uint32) \
   XX(CHILD_TASK_DELAY,uint32)

DECLARE_ENUM(Config,APP_CONFIG)


/******************************************************************************
** Command Macros
** - Commands implmented by child task are annotated with a comment
** - Load/dump table definitions are placeholders for JSON table
*/

#define MQTT_TBL_LOAD_CMD_FC                 (CMDMGR_APP_START_FC +  0)
#define MQTT_TBL_DUMP_CMD_FC                 (CMDMGR_APP_START_FC +  1)
#define MQTT_SB2MQ_REMOVE_ALL_PKTS_CMD_FC    (CMDMGR_APP_START_FC +  2)
#define MQTT_SB2MQ_REMOVE_PKT_CMD_FC         (CMDMGR_APP_START_FC +  3)
#define MQTT_SB2MQ_SEND_PKT_TBL_TLM_CMD_FC   (CMDMGR_APP_START_FC +  4)
#define MQTT_SET_RUN_LOOP_DELAY_CMD_FC       (CMDMGR_APP_START_FC +  5)

// #define MQTT_SB2MQ_UPDATE_FILTER_CMD_FC      (CMDMGR_APP_START_FC +  7)


/******************************************************************************
** Event Macros
**
** Define the base event message IDs used by each object/component used by the
** application. There are no automated checks to ensure an ID range is not
** exceeded so it is the developer's responsibility to verify the ranges. 
*/

#define MQTT_BASE_EID        (APP_FW_APP_BASE_EID +  0)
#define MQTT_MSG_BASE_EID    (APP_FW_APP_BASE_EID + 20)
#define MQ2SB_BASE_EID       (APP_FW_APP_BASE_EID + 30)
#define SB2MQ_BASE_EID       (APP_FW_APP_BASE_EID + 50)
#define MQTT_CLIENT_BASE_EID (APP_FW_APP_BASE_EID + 80)


/******************************************************************************
** MQTT Client
**
*/

#define MQTT_CLIENT_READ_BUF_LEN  2000 
#define MQTT_CLIENT_SEND_BUF_LEN  2000 
#define MQTT_CLIENT_TIMEOUT_MS    4000 

/******************************************************************************
** MQTT Client
**
*/
#define MQTT_STATS_STARTUP_INIT_MS   20000  /* ms after app initialized to start stats computations   */
#define SB2MQ_COMPUTE_STATS_INTERVAL_MS MQTT_MAX_RUN_LOOP_DELAY_MS


#endif /* _app_cfg_ */
