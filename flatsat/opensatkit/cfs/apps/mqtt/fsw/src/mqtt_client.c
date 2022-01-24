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

#include "mqtt_client.h"

/*****************/
/** Global Data **/
/*****************/

/******************************************************************************
** Function: MQTT_CLIENT_Constructor
**
** Notes:
**    1. This function must be called prior to any other functions being
**       called using the same MQTT_CLIENT instance.
*/
boolean MQTT_CLIENT_Constructor(MQTT_CLIENT_Class* MqttClientPtr,
                                const char *Hostname, int Port, 
                                const char *Clientname, 
                                const char *Username,
                                const char *Password,
                                uint16 KeepAliveSec,
                                uint16 YieldIntervalMsec,
                                boolean startClient)
{
   
   int RetCode;
   boolean RetStatus = FALSE;  /* TODO - What is a useful return value? Connecting during app init is not an error but may be good to know */ 


   // MqttClient = MqttClientPtr;
   
   CFE_PSP_MemSet((void*)MqttClientPtr, 0, sizeof(MQTT_CLIENT_Class));
   CFE_PSP_MemCpy(MqttClientPtr->Hostname, Hostname, strnlen(Hostname, MQTT_CLIENT_STR_LEN));
   CFE_PSP_MemCpy(MqttClientPtr->Clientname, Clientname, strnlen(Clientname, MQTT_CLIENT_STR_LEN));
   CFE_PSP_MemCpy(MqttClientPtr->Username, Username, strnlen(Username, MQTT_CLIENT_STR_LEN));
   CFE_PSP_MemCpy(MqttClientPtr->Password, Password, strnlen(Password, MQTT_CLIENT_STR_LEN));
   MqttClientPtr->KeepaliveSeconds = KeepAliveSec;
   MqttClientPtr->Port = Port;
   MqttClientPtr->ClientYieldIntervalMsec = YieldIntervalMsec;
   
   /*
   ** Init and connect to network
   */
   
   RetCode = MQTT_CLIENT_NetworkInitAndConnect(MqttClientPtr);

   MQTT_LOG_DEBUG("NetworkInitAndConnect Complete. RetCode: %d", RetCode);

   if (RetCode == 0) {

      /*
      ** Init MQTT client
      */

      MQTTClientInit(&MqttClientPtr->Client, 
                     &MqttClientPtr->Network, MQTT_CLIENT_TIMEOUT_MS,
                     MqttClientPtr->SendBuf,  MQTT_CLIENT_SEND_BUF_LEN,
                     MqttClientPtr->ReadBuf,  MQTT_CLIENT_READ_BUF_LEN); 

      MQTTPacket_connectData DefConnectOptions = MQTTPacket_connectData_initializer;
      CFE_PSP_MemCpy(&MqttClientPtr->ConnectData, &DefConnectOptions, sizeof(MQTTPacket_connectData));
      MqttClientPtr->ConnectData.username.cstring = MqttClientPtr->Username;
      MqttClientPtr->ConnectData.password.cstring = MqttClientPtr->Password;
      MqttClientPtr->ConnectData.clientID.cstring = MqttClientPtr->Clientname;
      MqttClientPtr->ConnectData.keepAliveInterval = KeepAliveSec * 1000;
      MqttClientPtr->ConnectData.willFlag = 0;
      MqttClientPtr->ConnectData.MQTTVersion = 3;
      MqttClientPtr->ConnectData.cleansession = 1;

      MQTT_LOG_DEBUG("Setup MQTT Connect Data. ClientID %s, User %s, Pass %d, Keepalive: %d", 
         MqttClientPtr->ConnectData.clientID.cstring, MqttClientPtr->ConnectData.username.cstring, 
         MqttClientPtr->ConnectData.password.cstring, MqttClientPtr->ConnectData.keepAliveInterval);

      if (startClient) {
         MQTT_LOG_INFO("Start MQTT Client on Init");
         RetStatus = MQTT_CLIENT_Connect(MqttClientPtr);
      }
      
   } /* End if successful NetworkConnect */
   else {
   
      CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_ERR_EID, CFE_EVS_ERROR, 
                        "Error creating network connection at %s:%d. Status=%d", Hostname, Port, RetCode);
   
   }

   MQTT_LOG_DEBUG("End of MQTT_CLIENT_Constructor(). RetStatus: %d", RetStatus);
   
   return RetStatus;

} /* End MQTT_CLIENT_Constructor() */

int MQTT_CLIENT_NetworkInitAndConnect(MQTT_CLIENT_Class* MqttClientPtr) {

   NetworkInit(&MqttClientPtr->Network);
   return NetworkConnect(&MqttClientPtr->Network, (char *)MqttClientPtr->Hostname, MqttClientPtr->Port);

}
/******************************************************************************
** Function: MQTT_CLIENT_Connect
**
**
*/
boolean MQTT_CLIENT_Connect(MQTT_CLIENT_Class* MqttClientPtr) {

   int RetCode;
   boolean RetStatus = FALSE;
   /*
   ** Connect to MQTT server
   */
   
   CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_EID, CFE_EVS_INFORMATION, 
                     "Connecting to MQTT broker %s:%d as client %s. User:Pass %s:%s", 
                     MqttClientPtr->Hostname, MqttClientPtr->Port, MqttClientPtr->Clientname, 
                     MqttClientPtr->ConnectData.username.cstring, MqttClientPtr->ConnectData.password.cstring);
   
   RetCode = MQTTConnect(&MqttClientPtr->Client, &MqttClientPtr->ConnectData);

   MQTT_LOG_INFO("MQTTConnect RetCode = %d", RetCode);

   if (RetCode == SUCCESS) {
      
      MqttClientPtr->Connected = TRUE;
      RetStatus = TRUE;
      
   }
   else {
      MQTT_LOG_ERROR("Error connecting to MQTT Broker");
      CFE_EVS_SendEvent(MQTT_CLIENT_CONSTRUCT_ERR_EID, CFE_EVS_ERROR, 
                        "Error connecting to MQTT broker at %s:%d as client %s. Status=%d", MqttClientPtr->Hostname, MqttClientPtr->Port, MqttClientPtr->Clientname, RetCode);
   
   }

   MQTT_LOG_DEBUG("End of MQTT_CLIENT_Connect() RetStatus: %d, Connect RetCode: %d", RetStatus, RetCode);

   return RetStatus;

} /* End MQTT_CLIENT_Constructor() */


/******************************************************************************
** Function: MQTT_CLIENT_Subscribe
**
** Notes:
**    1. QOS needs to be converted to MQTT library constants
*/

boolean MQTT_CLIENT_Subscribe(MQTT_CLIENT_Class* MqttClientPtr,
                              const char *Topic,
                              int Qos, 
                              MQTT_CLIENT_MsgCallback MsgCallbackFunc)
{
   
   boolean RetStatus = FALSE;
   
   int LibQos;

   if (Qos == MQTT_MSG_QOS0) {
      LibQos = QOS0;
   } else if (Qos == MQTT_MSG_QOS1) {
      LibQos = QOS1;
   } else {
      LibQos = QOS2;
   }

   RetStatus = (MQTTSubscribe(&MqttClientPtr->Client, Topic, LibQos, MsgCallbackFunc) == SUCCESS);

   if(RetStatus) {
      MqttClientPtr->SubscriptionCount++;
   }

   CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_INFORMATION, 
      "MQTT Client Subscribe (topic:qos) %s:%d. Status=%d", Topic, LibQos, RetStatus);

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

   int RetVal = MQTTUnsubscribe(&MqttClientPtr->Client, "#");

   if (RetVal != SUCCESS) {
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

   int RetVal = MQTTUnsubscribe(&MqttClientPtr->Client, topic);

   if (RetVal != SUCCESS) {
      RetStatus = FALSE;
   }

   CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_INFORMATION, 
      "MQTT Client Unsubscribe from topic %s, Status=%d", topic, RetStatus);

   return RetStatus;

} /* End MQTT_CLIENT_Unsubscribe() */

/******************************************************************************
** Function: MQTT_CLIENT_Yield
**
** Notes:
**    1. If yield fails, enforce a timeout to avoid CPU hogging
**
*/
boolean MQTT_CLIENT_Yield(MQTT_CLIENT_Class* MqttClientPtr)
{
   
   boolean RetStatus = FALSE;
   int rc = -1;
   MQTT_LOG_DEBUG("Start MQTT_CLIENT_Yield()");
   if (MQTTIsConnected(&MqttClientPtr->Client)) {
      rc = MQTTYield(&MqttClientPtr->Client, MqttClientPtr->ClientYieldIntervalMsec);
      // printf("MQTT_CLIENT_Yield rc = %d\n",rc);
         
      if (rc == SUCCESS) {
         RetStatus = TRUE;
      
      }
      else {
         // We got a yield error and the driver will disconnect us
         // Explicitly disconnect and then we'll try to reconnect
         if (!MQTTIsConnected(&MqttClientPtr->Client)) {
            MQTT_CLIENT_Disconnect(MqttClientPtr);

            MqttClientPtr->SubscriptionCount = 0;

            MqttClientPtr->Connected = MQTTIsConnected(&MqttClientPtr->Client);
             CFE_EVS_SendEvent(MQTT_CLIENT_YIELD_ERR_EID, CFE_EVS_ERROR, 
                          "MQTT client disconnect, reconnect on next cycle. Error rc = %d",rc);
         }
      }
   }
   else {
      MQTT_LOG_WARN("MQTT Client Not Connected During Attempt to Yield");
   }
   MQTT_LOG_DEBUG("End MQTT_CLIENT_Yield()");
   return RetStatus;
    
} /* End MQTT_CLIENT_Yield() */

boolean MQTT_CLIENT_Connected(MQTT_CLIENT_Class* MqttClientPtr) {
   return (boolean) MQTTIsConnected(&MqttClientPtr->Client);
}

/******************************************************************************
** Function: MQTT_CLIENT_Publish
**
** Notes:
**    Publish message to topic
**
*/
boolean MQTT_CLIENT_Publish(MQTT_CLIENT_Class* MqttClientPtr, const char* topic, const char* msg, size_t msgLen, uint16 qos, uint16 retained) {
   boolean RetStatus = FALSE;
   int rc = -1;

   MQTTMessage message = {
      .qos = qos,
      .retained = retained,
      .payload = (void*) msg,
      .payloadlen = msgLen
   };

   rc = MQTTPublish(&MqttClientPtr->Client, topic, &message);

   if (rc == SUCCESS) {
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
   
   MQTTDisconnect(&MqttClientPtr->Client);
   NetworkDisconnect(&MqttClientPtr->Network);

} /* End MQTT_CLIENT_Disconnect() */

