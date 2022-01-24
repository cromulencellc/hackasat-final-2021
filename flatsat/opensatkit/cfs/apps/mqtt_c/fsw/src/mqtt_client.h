/*
** Purpose: Define MQTT messages
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/
#ifndef _mqtt_client_
#define _mqtt_client_


/*
** Includes
*/

#include "app_cfg.h"
#include "mqtt_msg.h"
#include <mqtt.h>
// #include "mqtt.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define MQTT_CLIENT_CONSTRUCT_EID      (MQTT_CLIENT_BASE_EID + 0)
#define MQTT_CLIENT_CONSTRUCT_ERR_EID  (MQTT_CLIENT_BASE_EID + 1)
#define MQTT_CLIENT_YIELD_ERR_EID      (MQTT_CLIENT_BASE_EID + 2)
#define MQTT_CLIENT_SUB_EID            (MQTT_CLIENT_BASE_EID + 3)

/**********************/
/** Type Definitions **/
/**********************/

typedef void (*MQTT_CLIENT_SubscribeFcnPtr)(void);

typedef void (*publish_response_callback)(void **state, struct mqtt_response_publish *publish);

/*
** Class Definition
*/

typedef struct {

   boolean Connected;
   boolean Subscribed;
   uint16  SubscriptionCount;
   uint16  ReconnectCount;
   uint16  ClientYieldIntervalMsec;
   uint16  ClientKeepaliveSec;
   uint32  MsgPubCnt;
   uint32  MsgRecvCnt;
   const char *Hostname;
   const char *Port; 
   const char *Clientname;
   const char *Username;
   const char *Password;

   /*
   ** MQTT Library
   */
   MQTT_CLIENT_SubscribeFcnPtr SubscribeFcnPtr;
   int                     SocketFd;
   struct mqtt_client      Client;
   void (*publish_response_callback)(void **state, struct mqtt_response_publish *publish);
   unsigned char           SendBuf[MQTT_CLIENT_SEND_BUF_LEN];
   unsigned char           ReadBuf[MQTT_CLIENT_READ_BUF_LEN];

} MQTT_CLIENT_Class;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: MQTT_CLIENT_Constructor
**
** Notes:
**   1. This function must be called prior to any other functions being
**      called using the same cmdmgr instance.
*/
boolean MQTT_CLIENT_Constructor(MQTT_CLIENT_Class* MqttClientPtr,
                                const char *Hostname,
                                const char *Port, 
                                const char *Clientname, 
                                const char *Username,
                                const char *Password,
                                uint16 YieldIntervalMsec,
                                uint16 ClientKeepaliveSec,
                                publish_response_callback PublishResponseCallbackFcn,
                                MQTT_CLIENT_SubscribeFcnPtr SubscribeFcn);


/******************************************************************************
** Function: MQTT_CLIENT_Subscribe
**
** Notes:
**    1. QOS options are defined in mqtt_msg.h
*/
boolean MQTT_CLIENT_Subscribe(MQTT_CLIENT_Class* MqttClientPtr,
                              const char *Topic,
                              int Qos);


/******************************************************************************
** Function: MQTT_CLIENT_Unsubscribe
**
** Notes:
**    1. Unsubscribe from topic
*/
boolean MQTT_CLIENT_Unsubscribe(MQTT_CLIENT_Class* MqttClientPtr, const char* topic);


/******************************************************************************
** Function: MQTT_CLIENT_Publish
**
*/
boolean MQTT_CLIENT_Publish(MQTT_CLIENT_Class* MqttClientPtr, const char* topic, const char* msg, size_t msgLen, uint16 qos, uint16 retained);

/******************************************************************************
** Function: MQTT_CLIENT_Connected
**
*/
// boolean MQTT_CLIENT_Connected(MQTT_CLIENT_Class* MqttClientPtr);

/******************************************************************************
** Function: MQTT_CLIENT_Disconnect
**
*/
void MQTT_CLIENT_Disconnect(MQTT_CLIENT_Class* MqttClientPtr);

/******************************************************************************
** Function: MQTT_CLIENT_Refresh
**
*/
void MQTT_CLIENT_Refresh(MQTT_CLIENT_Class* MqttClientPtr);


#endif /* _mqtt_client_ */

