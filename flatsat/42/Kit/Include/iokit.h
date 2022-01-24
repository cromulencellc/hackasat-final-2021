/*    This file is distributed with 42,                               */
/*    the (mostly harmless) spacecraft dynamics simulation            */
/*    created by Eric Stoneking of NASA Goddard Space Flight Center   */

/*    Copyright 2010 United States Government                         */
/*    as represented by the Administrator                             */
/*    of the National Aeronautics and Space Administration.           */

/*    No copyright is claimed in the United States                    */
/*    under Title 17, U.S. Code.                                      */

/*    All Other Rights Reserved.                                      */


#ifndef __IOKIT_H__
#define __IOKIT_H__

/*
** #ifdef __cplusplus
** namespace Kit {
** #endif
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <limits.h>
#include <stdbool.h>


#ifdef _WIN32
   #include <winsock2.h>
#else
   #include <sys/socket.h>
   #include <netinet/in.h>
   #include <netinet/tcp.h>
   #include <netdb.h>
   /* Finesse winsock SOCKET datatype */
   #define SOCKET int
#endif

#ifdef _ENABLE_MQTT_
#include <string.h>
#include "MQTTClient.h"
#endif
/* #include <sys/un.h> */

FILE *FileOpen(const char *Path, const char *File, const char *CtrlCode);
void ByteSwapDouble(double *A);
int FileToString(const char *file_name, char **result_string,
                 size_t *string_len);

SOCKET InitSocketServer(int Port, int AllowBlocking);
SOCKET InitSocketClient(const char *hostname, int Port, int AllowBlocking);

#ifdef _ENABLE_MQTT_
#define MQTT_QOS         1
// #define MQTT_TIMEOUT_CNT 6
#define MQTT_RECV_TIMEOUT_MSEC 2000
#define MQTT_RECV_TIMEOUT_MAX  30000
#define MQTT_TIMEOUT           10000L
#define MQTT_TOPIC_PUB         "SIM/42/PUB"
#define MQTT_TOPIC_RECV        "SIM/42/RECV"
#define MQTT_CLIENT_NAME       "MqttClient42"
#define MQTT_USER              "42"
#define MQTT_PASS              "F99BD16958F2ACB72731BBA6"

MQTTClient InitMqttClient(const char* Host, int Port, const char* clientName, void* context);
int StartMqttClient(MQTTClient client, const char* username, const char* password, const char* topicRecv);
int MqttPublishMessage(MQTTClient client, char* data, unsigned int dataLen, const char* topic);
void CloseMqttClient(MQTTClient client);
int MqttMsgRecv(MQTTClient client, void* context);
#endif
/*
** #ifdef __cplusplus
** }
** #endif
*/

#endif /* __IOKIT_H__ */
