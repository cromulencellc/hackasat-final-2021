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

/*
** Include Files:
*/

#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <fcntl.h>
#include "mqtt_client.h"
#include "mqtt_logger.h"


/*****************/
/** Global Data **/
/*****************/

/**
 * @brief My reconnect callback. It will reestablish the connection whenever 
 *        an error occurs. 
 */
void reconnect_client(struct mqtt_client* client, void **reconnect_state_vptr);
boolean MQTT_CLIENT_InitReconnect(MQTT_CLIENT_Class* MqttClientPtr);
int MQTT_CLIENT_OpenSocket(MQTT_CLIENT_Class* MqttClientPtr);

/******************************************************************************
** Function: MQTT_CLIENT_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being
**       called using the same MQTT_CLIENT instance.
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
                                MQTT_CLIENT_SubscribeFcnPtr SubscribeFcn)
{
   
   boolean RetStatus = FALSE;  /* TODO - What is a useful return value? Connecting during app init is not an error but may be good to know */ 

   // MqttClient = MqttClientPtr;
   
   CFE_PSP_MemSet((void*)MqttClientPtr, 0, sizeof(MQTT_CLIENT_Class));
   
   MqttClientPtr->Hostname = Hostname;
   MqttClientPtr->Port = Port;
   MqttClientPtr->Clientname = Clientname;
   MqttClientPtr->ClientYieldIntervalMsec = YieldIntervalMsec;
   MqttClientPtr->ClientKeepaliveSec = ClientKeepaliveSec;
   MqttClientPtr->Username = Username;
   MqttClientPtr->Password = Password;
   MqttClientPtr->publish_response_callback = PublishResponseCallbackFcn;
   MqttClientPtr->SubscribeFcnPtr = SubscribeFcn;

   /*
   ** Init and connect to network
   */
   
   if (MQTT_CLIENT_InitReconnect(MqttClientPtr)) {

      CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_ERR_EID, CFE_EVS_INFORMATION, 
                        "MQTT Client Connect Success for %s:%s", Hostname, Port);
      
   } /* End if successful NetworkConnect */
   else {
   
      CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_ERR_EID, CFE_EVS_ERROR, 
                        "MQTT Client Connect Error for %s:%s", Hostname, Port);
   
   }
   
   return RetStatus;

} /* End MQTT_CLIENT_Constructor() */




int MQTT_CLIENT_OpenSocket(MQTT_CLIENT_Class* MqttClientPtr) {

   #ifndef MQTT_RTEMS
   /* open the non-blocking TCP socket (connecting to the broker) */
   struct addrinfo hints = {0};

   hints.ai_family = AF_UNSPEC; /* IPv4 or IPv6 */
   hints.ai_socktype = SOCK_STREAM; /* Must be TCP */
   int sockfd = -1;
   int rv;
   struct addrinfo *p, *servinfo;

   /* get address information */
   rv = getaddrinfo((char *)MqttClientPtr->Hostname, (char *)MqttClientPtr->Port, &hints, &servinfo);
   if(rv != 0) {
      MQTT_LOG_ERROR("Failed to open socket (getaddrinfo): %s\n", strerror(rv));
      return -1;
   }

   /* open the first possible socket */
   for(p = servinfo; p != NULL; p = p->ai_next) {
      sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol);
      if (sockfd == -1) continue;

      /* connect to server */
      rv = connect(sockfd, p->ai_addr, p->ai_addrlen);
      if(rv == -1) {
         close(sockfd);
         sockfd = -1;
         continue;
      }
      break;
   }  

   /* free servinfo */
   freeaddrinfo(servinfo);

   #else
   
   struct sockaddr_in address;
   int rc = -1;
   int sockfd = -1;

   if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
   {
      sockfd = -1;
   } 
   char *ptr;
   long port = strtol((char *)MqttClientPtr->Port, &ptr, 10);

   address.sin_port = htons(port);
   address.sin_family = AF_INET;

   if(inet_pton(AF_INET, (char *)MqttClientPtr->Hostname, &address.sin_addr)<=0) 
   {
      sockfd = -1;
   }

   if (sockfd != -1) {
      rc = connect(sockfd, (struct sockaddr*)&address, sizeof(address));
      if (rc < 0) {
         MQTT_LOG_ERROR("Could not connect RTEMS");
         sockfd = -1;
      }
   } 

   #endif

   if (sockfd != -1) fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFL) | O_NONBLOCK);

   return sockfd;  

}
/******************************************************************************
** Function: MQTT_CLIENT_Connect
**
**
*/
boolean MQTT_CLIENT_InitReconnect(MQTT_CLIENT_Class* MqttClientPtr) {

   boolean RetStatus = FALSE;
   /*
   ** Connect to MQTT server
   */
   
   CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_EID, CFE_EVS_INFORMATION, 
                     "Setup MQTT Reconnecting Client to MQTT broker %s:%s as client %s", MqttClientPtr->Hostname, MqttClientPtr->Port, MqttClientPtr->Clientname);
   
   mqtt_init_reconnect(&MqttClientPtr->Client, reconnect_client, MqttClientPtr, MqttClientPtr->publish_response_callback);

   return RetStatus;

} /* End MQTT_CLIENT_Constructor() */

void reconnect_client(struct mqtt_client* client, void **reconnect_state_vptr)
{
    MQTT_CLIENT_Class* MqttClientPtr = *((MQTT_CLIENT_Class**) reconnect_state_vptr);

    /* Close the clients socket if this isn't the initial reconnect call */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT) {
        close(client->socketfd);
    }

    /* Perform error handling here. */
    if (client->error != MQTT_ERROR_INITIAL_RECONNECT) {
        MQTT_LOG_ERROR("reconnect_client: called while client was in error state \"%s\"\n", 
               mqtt_error_str(client->error)
        );
    }

    /* Open a new socket. */
    int sockfd = MQTT_CLIENT_OpenSocket(MqttClientPtr);
    if (sockfd == -1) {
        MQTT_LOG_ERROR("Failed to open socket");
        return;
    }

    /* Reinitialize the client. */
    mqtt_reinit(client, sockfd, 
                MqttClientPtr->SendBuf, MQTT_CLIENT_SEND_BUF_LEN,
                MqttClientPtr->ReadBuf, MQTT_CLIENT_READ_BUF_LEN);

    /* Ensure we have a clean session */
    uint8_t connect_flags = MQTT_CONNECT_CLEAN_SESSION;

    /* Send connection request to the broker. */
    mqtt_connect(client, MqttClientPtr->Clientname, NULL, NULL, 0, MqttClientPtr->Username, MqttClientPtr->Password, connect_flags, MqttClientPtr->ClientKeepaliveSec);

    /* Subscribe to the topic. */
    MqttClientPtr->SubscribeFcnPtr();
}


/******************************************************************************
** Function: MQTT_CLIENT_Subscribe
**
** Notes:
**    1. QOS needs to be converted to MQTT library constants
*/

boolean MQTT_CLIENT_Subscribe(MQTT_CLIENT_Class* MqttClientPtr,
                              const char *Topic,
                              int Qos)
{
   
   boolean RetStatus = FALSE;
   
   if (mqtt_subscribe(&MqttClientPtr->Client, Topic, Qos) == MQTT_OK) {
      RetStatus = TRUE;
      CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_INFORMATION, "MQTT Client Subscribe Successful (topic:qos) %s:%d", Topic, Qos);
   } else {
      CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_ERROR, "MQTT Client Subscribe Error (topic:qos) %s:%d", Topic, Qos);
   }

   if(RetStatus) {
      MqttClientPtr->SubscriptionCount++;
   }

   return RetStatus;
   
} /* End MQTT_CLIENT_Subscribe() */


/******************************************************************************
** Function: MQTT_CLIENT_UnsubscribeAll()
**
** Notes:
**    1. Unsub from all subscriptions
*/
boolean MQTT_CLIENT_UnsubscribeAll(MQTT_CLIENT_Class* MqttClientPtr) {
   boolean RetStatus = FALSE;

   int RetVal = mqtt_unsubscribe(&MqttClientPtr->Client, "#");

   if (RetVal != MQTT_OK) {
      RetStatus = FALSE;
   }

   CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_INFORMATION, 
      "MQTT Client Unsubscribe from all topics  Status=%d", RetStatus);

   return RetStatus;

} /* End MQTT_CLIENT_UnsubscribeAll() */


/******************************************************************************
** Function: MQTT_CLIENT_Unsubscribe()
**
** Notes:
**    1. Unsub from all subscriptions
*/
boolean MQTT_CLIENT_Unsubscribe(MQTT_CLIENT_Class* MqttClientPtr, const char* topic) {
   boolean RetStatus = FALSE;

   int RetVal = mqtt_unsubscribe(&MqttClientPtr->Client, topic);

   if (RetVal != MQTT_OK) {
      RetStatus = FALSE;
   }

   CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_INFORMATION, 
      "MQTT Client Unsubscribe from topic %s, Status=%d", topic, RetStatus);

   return RetStatus;

} /* End MQTT_CLIENT_Unsubscribe() */


/******************************************************************************
** Function: MQTT_CLIENT_Publish
**
** Notes:
**    Publish message to topic
**
*/
boolean MQTT_CLIENT_Publish(MQTT_CLIENT_Class* MqttClientPtr, const char* topic, const char* msg, size_t msgLen, uint16 qos, uint16 retained) {
   boolean RetStatus = FALSE;
   uint8_t publish_flags = 0;
   switch (qos)
   {
   case MQTT_MSG_QOS0:
      publish_flags = MQTT_PUBLISH_QOS_0;
      break;
   case MQTT_MSG_QOS1:
      publish_flags = MQTT_PUBLISH_QOS_1;
      break;
   case MQTT_MSG_QOS2:
      publish_flags = MQTT_PUBLISH_QOS_2;
      break;
   default:
      break;
   }
   if (retained) {
      publish_flags |= MQTT_PUBLISH_RETAIN;
   }

   if (mqtt_publish(&MqttClientPtr->Client, topic, msg, msgLen, publish_flags) ==  MQTT_OK ) {
      RetStatus = TRUE;
   }

   return RetStatus;
}


/******************************************************************************
** Function: MQTT_CLIENT_Disconnect
**
** Notes:
**    None
**
*/
void MQTT_CLIENT_Disconnect(MQTT_CLIENT_Class* MqttClientPtr)
{
   mqtt_disconnect(&MqttClientPtr->Client);
} /* End MQTT_CLIENT_Disconnect() */


/******************************************************************************
** Function: MQTT_CLIENT_Refresh
**
*/
void MQTT_CLIENT_Refresh(MQTT_CLIENT_Class* MqttClientPtr) {
   if(mqtt_sync(&MqttClientPtr->Client) != MQTT_OK) {
      MQTT_LOG_ERROR("MQTT Sync Error");
   }
   OS_TaskDelay(MqttClientPtr->ClientYieldIntervalMsec);
}
