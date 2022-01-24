/*
** Purpose: Define the MQTT application
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**   2. The parent app owns all of the app objects and the child manager
**      manages the execute of one or more of these parent's objects. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/
#ifndef _mqtt_app_
#define _mqtt_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "childmgr.h"
#include "initbl.h"

#include "mqtt_msg.h"
#include "mqtt_client.h"
#include "sb2mq.h"
#include "mq2sb.h"

/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define MQTT_INIT_APP_EID                 (MQTT_BASE_EID + 0)
#define MQTT_NOOP_EID                     (MQTT_BASE_EID + 1)
#define MQTT_SET_RUNLOOP_DELAY_EID        (MQTT_BASE_EID + 2)
#define MQTT_INVALID_RUNLOOP_DELAY_EID    (MQTT_BASE_EID + 3)
#define MQTT_EXIT_EID                     (MQTT_BASE_EID + 4)
#define MQTT_INVALID_MID_EID              (MQTT_BASE_EID + 5)
#define MQTT_CHILD_TASK_DEBUG_EID         (MQTT_BASE_EID + 6)
#define MQTT_CHILD_TASK_SUB_ERROR_EID     (MQTT_BASE_EID + 7)
#define MQTT_CHILD_TASK_NOT_CONNECTED_EID (MQTT_BASE_EID + 8)
#define MQTT_CHILD_TASK_YIELD_EID         (MQTT_BASE_EID + 8)
/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** Child Task
   */
   uint16   ChildValidCmdCnt;
   uint16   ChildInvalidCmdCnt;

   /*
   ** Connection Stats
   */
   uint16   RecvConnected;
   uint16   RecvReconnectCount;
   uint16   RecvMsgCnt;
   uint16   RecvSubscriptionCount;
   uint16   PubMsgCnt;


} OS_PACK MQTT_HkPkt;
#define MQTT_TLM_HK_PKT_LEN  sizeof(MQTT_HkPkt)


/******************************************************************************
** MQTT_Class
*/
typedef struct {

   /*
   ** APP Runtime
   */
   uint16   RunLoopDelay;
   uint16   ChildTaskDelay;

   /* 
   ** App Framework
   */ 
   
   INITBL_Class    IniTbl; 
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class    CmdMgr;
   TBLMGR_Class    TblMgr;
   
   CHILDMGR_Class  ChildMgr;
      
   /*
   ** Telemetry Packets
   */
   
   MQTT_HkPkt  HkPkt;
   
   /*
   ** App Objects
   */
   MQTT_CLIENT_Class  MqttClient;
   MQ2SB_Class        Mq2Sb;
   SB2MQ_Class        Sb2Mq;
 
} MQTT_Class;


/*******************/
/** Exported Data **/
/*******************/

extern MQTT_Class  Mqtt;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: MQTT_AppMain
**
*/
void MQTT_AppMain(void);


/******************************************************************************
** Function: MQTT_NoOpCmd
**
*/
boolean MQTT_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: MQTT_ResetAppCmd
**
*/
boolean MQTT_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: MQTT_SetRunLoopDelayCmd
**
*/
boolean MQTT_SetRunLoopDelayCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   RunLoopDelay;

}  MQTT_SetRunLoopDelayCmdParam;
#define MQTT_SET_RUN_LOOP_DELAY_CMD_DATA_LEN  (sizeof(MQTT_SetRunLoopDelayCmdParam) - CFE_SB_CMD_HDR_SIZE)

/******************************************************************************
** Function: MQTT_SendHousekeepingPkt
**
*/
void MQTT_SendHousekeepingPkt(void);


#endif /* _mqtt_app_ */
