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
#ifndef _mqtt_msg_
#define _mqtt_msg_

/*
** Includes
*/

#include "cfe.h"

/***********************/
/** Macro Definitions **/
/***********************/
#define MQTT_MSG_QOS0        0
#define MQTT_MSG_QOS1        1
#define MQTT_MSG_QOS2        2


typedef enum {
   MQTT_MSG_STR    = 1,
   MQTT_MSG_JSON   = 2,
   MQTT_MSG_BINARY = 3,
} MqttDataTypeEnum_t;


/*
** Incoming MQTT Messages have 3 potential types
** 1. A single data item with a primitive type 
** 2. A CCSDS packet in JSON format
** 3. A data payload in JSON format ( no CCSDS header info )
**   Note: may not need to distinguish between 2 and 3
*/

typedef enum {
   MQTT_MSG_TYPE_UNDEF        = 0,
   MQTT_MSG_TYPE_PRIMITIVE    = 1,
   MQTT_MSG_TYPE_SB           = 2,
   MQTT_MSG_TYPE_JSON_PACKET  = 3,
   MQTT_MSG_TYPE_JSON_PAYLOAD = 4
} MqttMsgTypeEnum_t;



/**
 * MQTT STR Message Size Definition
 * 
*/
#define MQTT_MSG_STR_MAX_SIZE 2500


/*
** Events
*/

#define MQTT_MSG_CALLBACK_EID   (MQTT_MSG_BASE_EID + 0)




typedef struct {
   
   uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
   char     Data[MQTT_MSG_STR_MAX_SIZE];

} MQTT_MSG_SbPkt;
#define MQTT_MSG_SB_PKT_LEN sizeof(MQTT_MSG_SbPkt)
// #define MQTT_MSG_STR4000_TLM_PKT_LEN  sizeof(MQTT_MSG_Str4000TlmPkt)




#endif /* _mqtt_msg_ */
