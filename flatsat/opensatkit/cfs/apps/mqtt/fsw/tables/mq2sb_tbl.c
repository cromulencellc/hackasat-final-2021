/*
** Purpose: Define MQTT's MQTT-to-SB message table
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSat Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#include "cfe_tbl_filedef.h"
#include "app_cfg.h"
#include "mq2sb.h"
#include "mqtt_msg.h"


/*
** MQTT to Software Bus table (mqtt2sb) 
** 
** Each table entry has the following:
**  Type: MQTT_MSG_PKT_*
**  Message ID:
**  Data Type:
**  Data Length: (may not be needed)
*/
MQ2SB_TblData MQ2SB_Tbl =
{
   {

      /* 0 - Test with HiveMQ */
      {   
        .MsgType           = MQTT_MSG_TYPE_PRIMITIVE,
        .MsgId             = CFG_STR32_TLM_MID,      /* TODO - MQTT_STR32_TLM_MID */
        .MqttTopic         = "osk/1",
        .PktType           = MQTT_MSG_SB_MSG,
        .DataType          = MQTT_MSG_STR,
        .DataLength        = 32 
      },

      /* 1 - ADCS Sensor Data */
      {
         .MsgType           = MQTT_MSG_TYPE_PRIMITIVE,
         .MsgId             = 0x019E3,
         .MqttTopic         = "SIM/42/SENSOR",
         .PktType           = MQTT_MSG_SB_MSG,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 2000
      },

      /* 2 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_PRIMITIVE,
         .MsgId             = 0x019E4,
         .MqttTopic         = "COMM/PAYLOAD/SLA",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 200
      },

      /* 3 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_PRIMITIVE,
         .MsgId             = 0x019E5,
         .MqttTopic         = "COMM/PING/STATUS",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 200
      },

      /* 4 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_PRIMITIVE,
         .MsgId             = 0x019E6,
         .MqttTopic         = "COMM/PAYLOAD/TELEMETRY",
         .PktType           = MQTT_MSG_PKT_TLM,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 200
      },

      /* 5 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_SB_MSG,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 4
      },

      /* 6 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_SB_MSG,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 4
      },

      /* 7 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_SB_MSG,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 4
      },

      /* 8 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_SB_MSG,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 4
      },

      /* 9 - Unused */
      {
         .MsgType           = MQTT_MSG_TYPE_UNDEF,
         .MsgId             = 0,
         .MqttTopic         = "null",
         .PktType           = MQTT_MSG_SB_MSG,
         .DataType          = MQTT_MSG_STR,
         .DataLength        = 4
      }
    
   }
}; 

/*
** cFE table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "MQ2SB_Tbl", MQTT_APP_CFE_NAME "." MQTT_MQ2SB_TBL_CFE_NAME,
    "MQTT Msg to SB table", "mq2sb_tbl.tbl",sizeof(MQ2SB_TblData)
};
