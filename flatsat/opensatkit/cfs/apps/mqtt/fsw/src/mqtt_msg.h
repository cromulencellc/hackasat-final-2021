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

// #include "mq2sb.h"

#if defined(MQTTCLIENT_PLATFORM_HEADER)
/* The following sequence of macros converts the MQTTCLIENT_PLATFORM_HEADER value
 * into a string constant suitable for use with include.
 */
#define xstr(s) str(s)
#define str(s) #s
#include xstr(MQTTCLIENT_PLATFORM_HEADER)
#endif

#include "mqtt_lib.h"

/***********************/
/** Macro Definitions **/
/***********************/


#define MQTT_MSG_QOS0        0
#define MQTT_MSG_QOS1        1
#define MQTT_MSG_QOS2        2

// #define MQTT_MSG_INT8        8 
// #define MQTT_MSG_UINT8       9 
// #define MQTT_MSG_UINT16     15
// #define MQTT_MSG_INT16      16
// #define MQTT_MSG_UINT32     31
// #define MQTT_MSG_INT32      32
// #define MQTT_MSG_FLOAT      33
// #define MQTT_MSG_DOUBLE     64
// #define MQTT_MSG_STR16     116
// #define MQTT_MSG_STR32     132
// #define MQTT_MSG_STR64     164
// #define MQTT_MSG_STR128    200

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



/**********************/
/** Type Definitions **/
/**********************/

/*
** Int32 Payload
*/

// typedef struct {
   
//    uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    int32    Data;

// }  MQTT_MSG_Int32TlmPkt;
// #define MQTT_MSG_INT32_TLM_PKT_LEN  sizeof(MQTT_MSG_Int32TlmPkt)


// /*
// ** Float Payload
// */

// typedef struct {
   
//    uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    float    Data;

// }  MQTT_MSG_FloatTlmPkt;
// #define MQTT_MSG_FLOAT_TLM_PKT_LEN  sizeof(MQTT_MSG_FloatTlmPkt)


// /*
// ** Double Payload
// */

// typedef struct {
   
//    uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    double   Data;

// }  MQTT_MSG_DoubleTlmPkt;
// #define MQTT_MSG_DOUBLE_TLM_PKT_LEN  sizeof(MQTT_MSG_DoubleTlmPkt)


// /*
// ** String32 Payload
// */

// typedef struct {
   
//    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    char    Data[32];

// }  MQTT_MSG_Str32TlmPkt;
// #define MQTT_MSG_STR32_TLM_PKT_LEN  sizeof(MQTT_MSG_Str32TlmPkt)


// /*
// ** String64 Payload
// */

// typedef struct {
   
//    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    char    Data[64];

// }  MQTT_MSG_Str64TlmPkt;
// #define MQTT_MSG_STR64_TLM_PKT_LEN  sizeof(MQTT_MSG_Str64TlmPkt)


// /*
// ** String100 Payload
// */

// typedef struct {
   
//    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    char    Data[100];

// }  MQTT_MSG_Str100TlmPkt;
// #define MQTT_MSG_STR100_TLM_PKT_LEN  sizeof(MQTT_MSG_Str100TlmPkt)

// /*
// ** String800 Payload
// */

// typedef struct {
   
//    uint8   TlmHeader[CFE_SB_TLM_HDR_SIZE];
//    char    Data[1000];

// }  MQTT_MSG_Str1000TlmPkt;
// #define MQTT_MSG_STR1000_TLM_PKT_LEN  sizeof(MQTT_MSG_Str1000TlmPkt)

typedef struct {
   
   uint8    TlmHeader[CFE_SB_TLM_HDR_SIZE];
   char     Data[MQTT_MSG_STR_MAX_SIZE];

} MQTT_MSG_SbPkt;
#define MQTT_MSG_SB_PKT_LEN sizeof(MQTT_MSG_SbPkt)
// #define MQTT_MSG_STR4000_TLM_PKT_LEN  sizeof(MQTT_MSG_Str4000TlmPkt)


/*
** Class Definition
*/

// typedef struct {

//    /*
//    ** Framework References
//    */
   
//    INITBL_Class*  IniTbl;

//    /*
//    ** Table referencees
//    */
   
//    MQ2SB_TblData*  Mq2SbTbl;
//    SB2MQ_Tbl*  Sb2MqTbl;
   
//    /*
//    ** Telemetry Packets
//    */
//    MQTT_MSG_SbPkt         TlmPkt;
//    // MQTT_MSG_Int32TlmPkt    Int32TlmPkt;
//    // MQTT_MSG_FloatTlmPkt    FloatTlmPkt;
//    // MQTT_MSG_DoubleTlmPkt   DoubleTlmPkt;
//    // MQTT_MSG_Str32TlmPkt    Str32TlmPkt;
//    // MQTT_MSG_Str64TlmPkt    Str64TlmPkt;
//    // MQTT_MSG_Str100TlmPkt   Str100TlmPkt;

// } MQTT_MSG_Class;


#endif /* _mqtt_msg_ */
