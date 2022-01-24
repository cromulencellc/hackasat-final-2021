#include "cfe_tbl_filedef.h"
#include "app_cfg.h"
#include "sb2mq.h"
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
SB2MQ_TblData SB2MQ_Tbl =
{
    {
        /* 0 - ADCS Actuator Data */
        {
            .StreamId = 0x09E7,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 2,
            .MqttQos = 2,
            .MqttRetained = 0,
            .MqttTopic = "SIM/42/ACTUATOR",
            .State = 1
        },

        /* 1 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },

        /* 2 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },

        /* 3 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },

        /* 4 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },

        /* 5 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },

        /* 6 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },

        /* 7 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },

        /* 8 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },

        /* 9 - Unused */
        {
            .StreamId = SB2MQ_TBL_UNUSED_MSG_ID,
            .Qos = {
                .Priority = 0,
                .Reliability = 0
            },
            .BufferLimit = 0,
            .MqttQos = 0,
            .MqttRetained = 0,
            .MqttTopic = "",
            .State = 0
        },
    }
};

/*
** cFE table file header
*/
static CFE_TBL_FileDef_t CFE_TBL_FileDef __attribute__((__used__)) =
{
    "SB2MQ_Tbl", MQTT_APP_CFE_NAME "." MQTT_SB2MQ_TBL_CFE_NAME,
    "MQTT SB to Msg table", "sb2mq_tbl.tbl",sizeof(SB2MQ_Tbl)
};
