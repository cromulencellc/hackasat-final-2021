/*
** Purpose: Implement the MQTT-to-SB Table
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>
#include "app_cfg.h"
#include "mq2sb.h"
#include "mqtt_msg.h"
#include "mqtt_logger.h"

/*
** Global File Data
*/
#define  JSON  &(Mq2Sb->Json)  /* Convenience macro */
static MQ2SB_Class*  Mq2Sb = NULL;
static MQTT_CLIENT_Class* Mq2SbClient = NULL;
static MQ2SB_Tbl tempTbl;

/*
** Local Function Prototypes
*/
// void MQ2SB_ProcessSbMsg(MessageData* MsgData);


// static void MQ2SB_ComputeStats(uint16 EntrysSent, uint32 BytesSent);
/*
** Local File Function Prototypes
*/
static boolean WriteJsonEntry(int32 FileHandle, const MQ2SB_TblEntry* Entry, boolean FirstEntryWritten);
/******************************************************************************
** Function: JSON Callbacks
**
** Notes:
**   1. These functions must have the same function signature as 
**      JSON_ContainerFuncPtr.
*/
static boolean EntryCallback (void* UserData, int TokenIdx);


/******************************************************************************
** Function: MQ2SB_Constructor
**
*/
void MQ2SB_Constructor(MQ2SB_Class*  Mq2SbPtr,
                       MQTT_CLIENT_Class* Mq2SbClientPtr,
                       uint16 UpdateRate,
                       MQ2SB_GetTblPtrFcn    GetTblPtrFunc,
                       MQ2SB_LoadTblFcn      LoadTblFunc, 
                       MQ2SB_LoadTblEntryFcn LoadTblEntryFunc)
{
 
   Mq2Sb = Mq2SbPtr;
   Mq2SbClient = Mq2SbClientPtr;

   CFE_PSP_MemSet((void*)Mq2Sb, 0, sizeof(MQ2SB_Class));
   MQ2SB_SetTblToUnused(&(Mq2Sb->Tbl));

   Mq2Sb->StatsUpdateMsec = UpdateRate;
   Mq2Sb->GetTblPtrFunc    = GetTblPtrFunc;
   Mq2Sb->LoadTblFunc      = LoadTblFunc;
   Mq2Sb->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON, Mq2Sb->JsonFileBuf, Mq2Sb->JsonFileTokens);
   
   JSON_ObjConstructor(&(Mq2Sb->JsonObj[MQ2SB_OBJ_PKT]),
                       MQ2SB_OBJ_NAME_PKT,
                       EntryCallback,
                       (void *)&(Mq2Sb->Tbl.Entry));

   JSON_RegContainerCallback(JSON, &(Mq2Sb->JsonObj[MQ2SB_OBJ_PKT]));
   

} /* End MQ2SB_Constructor() */


/******************************************************************************
** Function:  MQ2SB_ResetStatus
**
*/
void MQ2SB_ResetStatus()
{

   /* Nothing to do */

} /* End MQ2SB_ResetStatus() */


/******************************************************************************
** Function:  ConvertMqToCcsds
**
** TODO - Think thorugh SB packet init and table loads.
** TODO - I added message ID defs to JSON ini file so the cFE table doesn't
**        contain the actual message ID but a ini table config ID. I've only
**        changed STR32 below since this is still in prototype stage
*/
static CFE_SB_Msg_t* ConvertMqToCcsds(MQ2SB_TblEntry* Mq2SbTbl, char* Payload, size_t PayloadSize)
{
  
   // CFE_SB_Msg_t*  CcsdsMsgPtr = NULL;
   MQTT_MSG_SbPkt* SbPktPtr = &Mq2Sb->SbPkt;

   // int32    MqInt32;
   // float    MqFloat;
   // double   MqDouble;

   // CFE_SB_MsgId_t MsgId = CFE_SB_INVALID_MSG_ID;

   /*~~
   CFE_EVS_SendEvent(MQTT_MSG_CALLBACK_EID, CFE_EVS_INFORMATION,
                     "ConvertMqToCcsds: MessageType=%d",Mq2SbTbl->MsgType);
   */
   switch (Mq2SbTbl->MsgType) {

      case MQTT_MSG_TYPE_PRIMITIVE:

         switch (Mq2SbTbl->DataType) {
            
            // int Status;

            case MQTT_MSG_STR:
               // Check that Payload size is compliant
               if (PayloadSize <= Mq2SbTbl->DataLength) {
                  CFE_SB_InitMsg((CFE_SB_Msg_t*) SbPktPtr, Mq2SbTbl->MsgId, MQTT_MSG_SB_PKT_LEN, TRUE);
                  // Set Payload Data
                  CFE_PSP_MemCpy(SbPktPtr->Data, Payload, PayloadSize);
               } else {
                  CFE_EVS_SendEvent(MQTT_MSG_CALLBACK_EID, CFE_EVS_ERROR, "MQTT MQTT_MSG_STR msg recv of %d bytes != %d tbl value", 
                                    PayloadSize, Mq2SbTbl->DataLength);
               }
               break;

            default:
               MQTT_LOG_ERROR("MQTT --> ERROR: Unknown type!\n");
         
         } /* End DataType switch */
         break;
      
      case MQTT_MSG_TYPE_JSON_PACKET:
         MQTT_LOG_INFO("MQTT JSON Packet Message Not Implemented\n");
         break;

      case MQTT_MSG_TYPE_JSON_PAYLOAD:
         MQTT_LOG_INFO("MQTT JSON Payload Message Not Implemented\n");
         break;
      default:
         MQTT_LOG_ERROR("Unknown Message Type\n");

   } /* End MsgType switch */
   
   return (CFE_SB_Msg_t*) SbPktPtr;

} /* End ConvertMqToCcsds() */

int MQ2SB_GetTblEntryIdxByMqttTopic(const char* topic) {
   int i = 0;
   size_t topicLen = strlen(topic);
   for (int i=0; i<MQTT_MQ2SB_TBL_ENTRIES; i++ ) {
      //~~ OS_printf("Table topic length=%d\n",TopicLen);
      if (strncmp(Mq2Sb->Tbl.Entry[i].MqttTopic, topic, topicLen) == 0) {
         break;
      } 
      else {
         i++;
      }
   }
   return i;
}

/******************************************************************************
** Function: MQTT_MSG_ProcessMsg
**
** Notes:
**   1. Signature must match MQTT_CLIENT_MsgCallback
**   2. MQTT has no delimeter between the topic and payload
**
*/
void MQ2SB_ProcessMsg(void** unused, struct mqtt_response_publish *published)
{
      
   int16    i = 0;
   boolean  TblEnd   = FALSE;
   boolean  MsgFound = FALSE;
   MQ2SB_Tbl*  Mq2SbTbl = &Mq2Sb->Tbl;

   if(published->application_message_size > 0) {

       /* note that published->topic_name is NOT null-terminated (here we'll change it to a c-string) */
      size_t topic_name_size = published->topic_name_size + 1;
      char* topic_name = (char*) malloc(topic_name_size);
      memcpy(topic_name, published->topic_name, published->topic_name_size);
      topic_name[published->topic_name_size] = '\0';
      
      //~~ OS_printf("MsgPtr->payloadlen=%d\n",MsgPtr->payloadlen);
      while ( MsgFound == FALSE && TblEnd == FALSE && i < MQTT_MQ2SB_TBL_ENTRIES) {
         
         if (Mq2SbTbl->Entry[i].MsgType == MQTT_MSG_TYPE_UNDEF) {
            
            TblEnd = TRUE;

         }
         else {
            
            int TopicLen = strlen(Mq2SbTbl->Entry[i].MqttTopic);
            
            //~~ OS_printf("Table topic length=%d\n",TopicLen);
            if (strncmp(Mq2SbTbl->Entry[i].MqttTopic, topic_name, TopicLen) == 0) {
               MsgFound = TRUE;
               CFE_EVS_SendEvent(MQTT_MSG_CALLBACK_EID, CFE_EVS_DEBUG, "MQTT_MSG_ProcessMsg: Topic=%s, SB MsgId=0x%x,", 
                                 topic_name, Mq2SbTbl->Entry[i].MsgId);
            } 
            else {
               i++;
            }
         }

      } /* End while loop */
      free(topic_name);

      if (MsgFound) {
            
         // char  PayloadStr[80];
         CFE_SB_Msg_t* SbMsgPtr;
         
         // strncpy(PayloadStr, MsgPtr->payload, MsgPtr->payloadlen); 
         // PayloadStr[MsgPtr->payloadlen] = 0;
         
         SbMsgPtr = ConvertMqToCcsds(&Mq2SbTbl->Entry[i], (char *)published->application_message, published->application_message_size);
          
         if (SbMsgPtr != NULL) {
            CFE_SB_TimeStampMsg(SbMsgPtr);
            CFE_SB_SendMsg(SbMsgPtr);
         }
         
      } /* End if message found */
      else if (TblEnd == TRUE) {
      
         MQTT_LOG_ERROR("MQTT --> Message not found before end of table!\n");
      
      }
   
   } /* End null message len */
   else {
      
     MQTT_LOG_ERROR("MQ2SB_ProcessMsg() Message Length Zero or NULL Ptr");
    
   }
   
   fflush(stdout);

} /* End MQTT_MSG_ProcessMsg() */


/******************************************************************************
** Function: MQTT_CLIENT_SubscribeMq2Sb
**
*/
void MQTT_CLIENT_SubscribeMq2Sb(void)
{
   
   int    i;
   int    SubscribeErr = 0;
   MQ2SB_Tbl* Mq2SbTbl = &Mq2Sb->Tbl;

   // Clear out current subs first. Otherwise on table load we could have extras over time.
   // MQTT_CLIENT_UnsubscribeAll(&Mqtt.MqttRecvClient);
   
   for (i = 0; i < MQTT_MQ2SB_TBL_ENTRIES; i++) {

      if (Mq2SbTbl->Entry[i].MsgType == MQTT_MSG_TYPE_PRIMITIVE) {
      
         if (!MQTT_CLIENT_Subscribe(Mq2SbClient,
                                    Mq2SbTbl->Entry[i].MqttTopic, 
                                    Mq2SbTbl->Entry[i].MqttQos)) {
            
            ++SubscribeErr;
            CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_ERROR,
                              "Error subscribing to MQ2SB table entry %d, topic %s, QOS %d",
                              i, Mq2SbTbl->Entry[i].MqttTopic, MQTT_MSG_QOS2);
            
         } 
      
      } else if (Mq2SbTbl->Entry[i].MsgType == MQTT_MSG_TYPE_SB)
      {
         if (!MQTT_CLIENT_Subscribe(Mq2SbClient,
                                    Mq2SbTbl->Entry[i].MqttTopic, 
                                    Mq2SbTbl->Entry[i].MqttQos)) {
            
            ++SubscribeErr;
            CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_ERROR,
                              "Error subscribing to MQ2SB table entry %d, topic %s, QOS %d",
                              i, Mq2SbTbl->Entry[i].MqttTopic, MQTT_MSG_QOS2);
            
         } 
      }
      else {
         
         /* Assume the first unused entry is the end of the table */ 
         break;
      
      }
      
   } /* End for loop */
   
   CFE_EVS_SendEvent(MQTT_CLIENT_SUB_EID, CFE_EVS_INFORMATION,
                     "Subscribed to %d MQ2SB table topics with %d errors",
                     (i-SubscribeErr), SubscribeErr);
   
} /* End MqttRecvClientSubscribe() */


/******************************************************************************
** Function: MQ2SB_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean MQ2SB_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int EntryIdx;
   
   CFE_EVS_SendEvent(MQ2SB_INIT_DEBUG_EID, MQ2SB_INIT_EVS_TYPE, "MQ2SB_LoadCmd() Entry. sizeof(Mq2Sb->Tbl) = %d\n", sizeof(Mq2Sb->Tbl));
   
   /* 
   ** Reset status, object modified flags, and data. A non-zero BufLim
   ** value is used to determine whether a packet was loaded.
   */
   MQ2SB_ResetStatus();  
   
   MQ2SB_SetTblToUnused(&tempTbl);

   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(MQ2SB_INIT_DEBUG_EID, MQ2SB_INIT_EVS_TYPE, "MQ2SB_LoadCmd() - Successfully prepared file %s\n", Filename);
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
    
      JSON_ProcessTokens(JSON);

      /* 
      ** Only process command if no attribute errors. No need to send an event
      ** message if there are attribute errors since events are sent for each
      ** error.  Checking at least one packet is a simple check but that's all
      ** that can be done because the table doesn't have any other constraints.
      ** A table with no packets is considered erroneous.
      */
      if (Mq2Sb->AttrErrCnt == 0) {
      
         if (Mq2Sb->EntryLoadCnt > 0) {
         
            if (LoadType == TBLMGR_LOAD_TBL_REPLACE) {
         
			      Mq2Sb->LastLoadStatus = ((Mq2Sb->LoadTblFunc)(&(tempTbl)) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

	         } /* End if replace entire table */
            else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
         
		         Mq2Sb->LastLoadStatus = TBLMGR_STATUS_VALID;
   
               if (Mq2Sb->JsonObj[MQ2SB_OBJ_PKT].Modified) {
                         
                  for (EntryIdx=0; (EntryIdx < MQTT_MQ2SB_TBL_ENTRIES) && (Mq2Sb->LastLoadStatus == TBLMGR_STATUS_VALID); EntryIdx++) {
                          
                     if (Mq2Sb->Tbl.Entry[EntryIdx].MsgId != MQ2SB_UNUSED_MSG_ID) {

                        if (!(Mq2Sb->LoadTblEntryFunc)(EntryIdx, (MQ2SB_TblEntry*)Mq2Sb->JsonObj[MQ2SB_OBJ_PKT].UserData))
                            Mq2Sb->LastLoadStatus = TBLMGR_STATUS_INVALID;
                     }     
                  } /* End packet array loop */                
               } /* End if at least one object modified */
               else {
                  
                  /* This is an un explainable error */
                  CFE_EVS_SendEvent(MQ2SB_LOAD_EMPTY_ERR_EID, CFE_EVS_ERROR, "Load packet table update rejected.");

               } /* End if no objects in file */
               
            } /* End if update records */
			   else {
               
               CFE_EVS_SendEvent(MQ2SB_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"Load packet table rejected. Invalid table command load type %d", LoadType);            
            
            } /* End if invalid command option */ 
            
         } /* End if at least one packet loaded */
         else {
            
            CFE_EVS_SendEvent(MQ2SB_LOAD_UPDATE_ERR_EID, CFE_EVS_ERROR,
			                     "Load packet table command rejected. %s didn't contain any packet defintions", Filename);
         
         } /* End if too many packets in table file */

      } /* End if no attribute errors */
            
   } /* End if valid file */
   else {
      
      //printf("**ERROR** Processing Packet Table file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(MQ2SB_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"Load packet table open failure for file %s. File Status = %s JSMN Status = %s",
	                     Filename, JSON_GetFileStatusStr(Mq2Sb->Json.FileStatus), JSON_GetJsmnErrStr(Mq2Sb->Json.JsmnStatus));
   
   } /* End if file processing error */

    
   return (Mq2Sb->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of MQ2SB_LoadCmd() */


/******************************************************************************
** Function: MQ2SB_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. DumpType is unused.
**  4. File is formatted so it can be used as a load file. It does not follow
**     the cFE table file format. 
**  5. Creates a new dump file, overwriting anything that may have existed
**     previously
*/

boolean MQ2SB_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   boolean  FirstEntryWritten = FALSE;
   int32    FileHandle;
   uint16   EntryIdx;
   char     DumpRecord[256];
   const MQ2SB_Tbl *Mq2SbPtr;
   char SysTimeStr[128];
   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      Mq2SbPtr = (Mq2Sb->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"MQTT MQ2SB Packet Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      
      sprintf(DumpRecord,"\"description\": \"SBN2MQ Tbl dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));


      /* 
      ** Packet Array 
      **
      ** - Not all fields in ground table are saved in FSW so they are not
      **   populated in the dump file. However, the dump file can still
      **   be loaded.
      */
      
      sprintf(DumpRecord,"\"entry-array\": [\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      for (EntryIdx=0; EntryIdx < MQTT_MQ2SB_TBL_ENTRIES; EntryIdx++) {
               
         if (WriteJsonEntry(FileHandle, &(Mq2SbPtr->Entry[EntryIdx]), FirstEntryWritten)) FirstEntryWritten = TRUE;
              
      } /* End packet loop */

      sprintf(DumpRecord,"\n]}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(MQ2SB_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of MQ2SB_DumpCmd() */



/******************************************************************************
** Function: WriteJsonEntry
**
** Notes:
**   1. Can't end last record with a comma so logic checks that commas only
**      start to be written after the first packet has been written
*/
static boolean WriteJsonEntry(int32 FileHandle, const MQ2SB_TblEntry* Entry, boolean FirstEntryWritten) {
   
   boolean EntryWritten = FALSE;
   char DumpRecord[256];

   if (Entry->MsgId != MQ2SB_UNUSED_MSG_ID && Entry->MsgType != MQTT_MSG_TYPE_UNDEF) {
      
      if (FirstEntryWritten) {
         sprintf(DumpRecord,",\n");
         OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      }
      
      sprintf(DumpRecord,"\"mqtt-message\": {\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"   \"msg-type\": %d,\n   \"dec-id\": %d,\n   \"data-type\": %d,\n   \"data-length\": %d,\n   \"mqtt-qos\": %d,\n   \"mqtt-topic\": %s\n}",
              Entry->MsgType, Entry->MsgId, Entry->DataType, Entry->DataLength, Entry->MqttQos, Entry->MqttTopic);

      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      EntryWritten = TRUE;
      
   } /* End if MsgId record has been loaded */
   
   return EntryWritten;
   
} /* End WriteJsonEntry() */


/******************************************************************************
** Function: EntryCallback
**
** Process a packet table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. ObjLoadCnt incremented for every packet, valid or invalid.
**      EntryLoadIdx index to stored new pkt and incremented for valid packets
*/
static boolean EntryCallback (void* UserData, int TokenIdx)
{

   int  AttributeCnt = 0;
   int  JsonIntData;
   char JsonStrData[MQTT_TOPIC_LEN];
   MQ2SB_TblEntry Entry;
   
   Mq2Sb->JsonObj[MQ2SB_OBJ_PKT].Modified = FALSE; 
   
   CFE_EVS_SendEvent(MQ2SB_INIT_DEBUG_EID, MQ2SB_INIT_EVS_TYPE,
                     "MQ2SB.EntryCallback: EntryLoadCnt %d, AttrErrCnt %d, TokenIdx %d",
                     Mq2Sb->EntryLoadCnt, Mq2Sb->AttrErrCnt, TokenIdx);
      
   MQ2SB_SetEntryToUnused(&Entry); /* Default filter to always filtered in case not loaded by FilterCallback() */
   
   if (JSON_GetValShortInt(JSON, TokenIdx, "dec-id",      &JsonIntData)) { AttributeCnt++; Entry.MsgId        = (CFE_SB_MsgId_t) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "msg-type",      &JsonIntData)) { AttributeCnt++; Entry.MsgType        = (CFE_SB_MsgId_t) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "data-type",   &JsonIntData)) { AttributeCnt++; Entry.DataType = (uint16)JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "data-length",   &JsonIntData)) { AttributeCnt++; Entry.DataLength = (uint16)JsonIntData; }
   if (JSON_GetValStr(JSON, TokenIdx, "mqtt-topic", JsonStrData)) { AttributeCnt++; CFE_PSP_MemCpy(Entry.MqttTopic, JsonStrData, MQTT_TOPIC_LEN);}
   if (JSON_GetValShortInt(JSON, TokenIdx, "mqtt-qos",      &JsonIntData)) { AttributeCnt++; Entry.MqttQos        = (CFE_SB_MsgId_t) JsonIntData; }


   if (AttributeCnt == 6) {
   
      ++Mq2Sb->EntryLoadCnt;
      
      Mq2Sb->CurMsgId = Entry.MsgId & MQ2SB_APP_ID_MASK;     
      CFE_PSP_MemCpy(&tempTbl.Entry[Mq2Sb->TblMsgCount], &Entry, sizeof(MQ2SB_TblEntry));   

      ++Mq2Sb->TblMsgCount;
      
      Mq2Sb->JsonObj[MQ2SB_OBJ_PKT].Modified = TRUE;
      
      CFE_EVS_SendEvent(MQ2SB_INIT_DEBUG_EID, MQ2SB_INIT_EVS_TYPE, 
                        "MQ2SB.EntryCallback (Stream Msg ID, Msg Type, Data Type, Data Len, MQTT QoS, MQTT Topic): %d, %d, %d, %d, %d, %s",
                        Entry.MsgId, Entry.MsgType, Entry.DataType, Entry.DataLength, Entry.MqttQos, Entry.MqttTopic);
   
   } /* End if valid AttributeCnt */
   else {
	   
      ++Mq2Sb->AttrErrCnt;     
      CFE_EVS_SendEvent(MQ2SB_LOAD_PKT_ATTR_ERR_EID, CFE_EVS_ERROR, "Invalid number of packet attributes %d. Should be 6.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */
      
   return Mq2Sb->JsonObj[MQ2SB_OBJ_PKT].Modified;

} /* EntryCallback() */


boolean MQ2SB_TblEntryValidate(MQ2SB_TblEntry* Entry) {\
   boolean valid = TRUE;
   if (Entry->MsgId == MQ2SB_UNUSED_MSG_ID) valid &= FALSE;
   if (Entry->DataLength == 0 && Entry->MsgType != MQTT_MSG_TYPE_SB) valid &= FALSE;
   if (Entry->DataLength > MQTT_CLIENT_READ_BUF_LEN) valid &= FALSE;
   if (Entry->DataType > 3) valid &= FALSE;
   if (!(Entry->MsgType == MQTT_MSG_TYPE_PRIMITIVE || Entry->MsgType == MQTT_MSG_TYPE_SB)) valid &= FALSE; // MQTT_MSG_TYPE_PRIMITIVE only suported type right now
   if (Entry->MqttQos > 2) valid &= FALSE;
   if (strlen(Entry->MqttTopic) <= 0) valid &= FALSE;
   return valid;
}

/******************************************************************************
** Function: MQ2SB_GetTblPtr
**
*/
const MQ2SB_Tbl* MQ2SB_GetTblPtr()
{

   return &(Mq2Sb->Tbl);

} /* End MQ2SB_GetTblPtr() */

/******************************************************************************
** Function: MQ2SB_LoadTbl
**
*/
boolean MQ2SB_LoadTbl(MQ2SB_Tbl* NewTbl)
{

   uint16   EntryIdx;
   uint16   PktCnt = 0;
   uint16   InvalidCnt = 0;
   boolean  RetStatus = TRUE;

   CFE_SB_MsgPtr_t MsgPtr = NULL;

   if(Mq2SbClient->Subscribed) {
      MQ2SB_RemoveAllPktsCmd(NULL, MsgPtr);  /* Both parameters are unused so OK to be NULL */
   }
   Mq2SbClient->Subscribed = FALSE;

   CFE_PSP_MemCpy(&(Mq2Sb->Tbl), NewTbl, sizeof(MQ2SB_Tbl));

   for (EntryIdx=0; EntryIdx < MQTT_MQ2SB_TBL_ENTRIES; EntryIdx++) {
      if (Mq2Sb->Tbl.Entry[EntryIdx].MsgId != MQ2SB_UNUSED_MSG_ID && Mq2Sb->Tbl.Entry[EntryIdx].MsgType != MQTT_MSG_TYPE_UNDEF ) {
         if(MQ2SB_TblEntryValidate(&(Mq2Sb->Tbl.Entry[EntryIdx]))) {
            ++PktCnt;
         }  else {
            ++InvalidCnt;
         }
      }
   } /* End pkt loop */

   if (InvalidCnt == 0) {
      Mq2Sb->LastLoadStatus = TRUE;

      MQ2SB_InitStats(Mq2Sb->StatsUpdateMsec,MQTT_STATS_STARTUP_INIT_MS);
      CFE_EVS_SendEvent(MQ2SB_LOAD_TBL_INFO_EID, CFE_EVS_INFORMATION,
                        "Successfully loaded new table with %d messages", PktCnt);

   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(MQ2SB_LOAD_TBL_ERR_EID, CFE_EVS_INFORMATION,
                        "Attempted to load new table with %d messages. %d messages invalid",
                        PktCnt, InvalidCnt);
   }

   return RetStatus;

} /* End MQ2SB_LoadTbl() */

/******************************************************************************
** Function: MQ2SB_LoadTblEntry
**
*/
boolean MQ2SB_LoadTblEntry(uint16 EntryIdx, MQ2SB_TblEntry* EntryArray)
{
   boolean  RetStatus = TRUE;
   MQ2SB_TblEntry* NewPkt = &(Mq2Sb->Tbl.Entry[EntryIdx]); 

   if(MQ2SB_TblEntryValidate(&(EntryArray[EntryIdx])))
   {
      CFE_PSP_MemCpy(NewPkt,&EntryArray[EntryIdx],sizeof(MQ2SB_TblEntry));
      CFE_EVS_SendEvent(MQ2SB_LOAD_TBL_ENTRY_INFO_EID, CFE_EVS_INFORMATION,
                        "Loaded table entry %d with MsgId 0x%04X, Mqtt Topic %s",
                        EntryIdx, NewPkt->MsgId, NewPkt->MqttTopic);
   } else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(MQ2SB_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID,CFE_EVS_ERROR,
                        "Error Loading table entry %d with MsgId 0x%04X, Mqtt Topic %s",
                        EntryIdx, NewPkt->MsgId, NewPkt->MqttTopic);
   }

   return RetStatus;

} /* End MQ2SB_LoadTblEntry() */

/******************************************************************************
** Function: MQ2SB_SetEntryToUnused
**
**
*/
void MQ2SB_SetEntryToUnused(MQ2SB_TblEntry* EntryPtr)
{

   CFE_PSP_MemSet(EntryPtr, 0, sizeof(MQ2SB_TblEntry));
   EntryPtr->MsgType = MQTT_MSG_TYPE_UNDEF;
   EntryPtr->MsgId    = MQ2SB_UNUSED_MSG_ID;
   
} /* End SBNTBL_SetPacketToUnused() */

/******************************************************************************
** Function: MQ2SB_SetTblToUnused
**
**
*/
void MQ2SB_SetTblToUnused(MQ2SB_Tbl* TblPtr)
{
  
   uint16 EntryIdx;
   
   CFE_PSP_MemSet(TblPtr, 0, sizeof(MQ2SB_Tbl));

   for (EntryIdx=0; EntryIdx < MQTT_MQ2SB_TBL_ENTRIES; EntryIdx++) {
      
      TblPtr->Entry[EntryIdx].MsgId    = MQ2SB_UNUSED_MSG_ID;
      TblPtr->Entry[EntryIdx].MsgType = MQTT_MSG_TYPE_UNDEF;
   
   }
   
} /* End MQ2SB_SetTblToUnused() */

/*******************************************************************
** Function: MQ2SB_RemovePktCmd
**
*/
boolean MQ2SB_RemovePktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{
   const MQ2SB_RemovePktCmdParam *RemovePktCmd = (const MQ2SB_RemovePktCmdParam *) MsgPtr;
   boolean  RetStatus = TRUE;
  
  int EntryIdx = MQ2SB_GetTblEntryIdxByMqttTopic(RemovePktCmd->MqttTopic);
   
  
   if ( Mq2Sb->Tbl.Entry[EntryIdx].MsgId != MQ2SB_UNUSED_MSG_ID) {

      MQ2SB_SetEntryToUnused(&(Mq2Sb->Tbl.Entry[EntryIdx]));

      CFE_EVS_SendEvent(MQ2SB_REMOVE_PKT_SUCCESS_EID, CFE_EVS_INFORMATION,
                  "Succesfully removed stream id 0x%04X with MQTT Topic %s from the message table",
                  Mq2Sb->Tbl.Entry[EntryIdx].MsgId, Mq2Sb->Tbl.Entry[EntryIdx].MqttTopic);

   } /* End if found stream ID in table */
   else
   {

      CFE_EVS_SendEvent(MQ2SB_REMOVE_PKT_ERROR_EID, CFE_EVS_ERROR,
                        "Error removing message with MQTT Topic %s. Message not defined in table.",
                        RemovePktCmd->MqttTopic);

   } /* End if didn't find stream ID in table */

   return RetStatus;

} /* End of MQ2SB_RemovePktCmd() */

/******************************************************************************
** Function: MQ2SB_RemoveAllPktsCmd
**
** Notes:
**   1. The cFE to_lab code unsubscribes the command and send HK MIDs. I'm not
**      sure why this is done and I'm not sure how the command is used. This
**      comman
*/
boolean MQ2SB_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr)
{

   uint16   TblIdx;
   uint16   PktCnt = 0;
   uint16   FailedUnsubscribe = 0;
   int32    Status;
   boolean  RetStatus = TRUE;

   for (TblIdx=0; TblIdx < MQTT_MQ2SB_TBL_ENTRIES; TblIdx++) {
      
      if (Mq2Sb->Tbl.Entry[TblIdx].MsgId != MQ2SB_UNUSED_MSG_ID && Mq2Sb->Tbl.Entry[TblIdx].MsgType != MQTT_MSG_TYPE_UNDEF) {
          
         ++PktCnt;

         Status = MQTT_CLIENT_Unsubscribe(Mq2SbClient, Mq2Sb->Tbl.Entry[TblIdx].MqttTopic);
         if(Status != CFE_SUCCESS) {
             
            FailedUnsubscribe++;
            CFE_EVS_SendEvent(MQ2SB_REMOVE_ALL_PKTS_ERROR_EID, CFE_EVS_ERROR,
                              "Error removing msg id 0x%04X for mqtt topic %s at table packet index %d. Unsubscribe status 0x%8X",
                              Mq2Sb->Tbl.Entry[TblIdx].MsgId, Mq2Sb->Tbl.Entry[TblIdx].MqttTopic, TblIdx, Status);
         }
         // MQ2SB_SetEntryToUnused(&(Mq2Sb->Tbl.Pkt[TblIdx]));

      } /* End if packet in use */

   } /* End AppId loop */

   if (FailedUnsubscribe == 0) {
      CFE_EVS_SendEvent(MQ2SB_REMOVE_ALL_PKTS_SUCCESS_EID, CFE_EVS_INFORMATION,
                        "Removed %d table packet entries", PktCnt);
   }
   else {
      RetStatus = FALSE;
      CFE_EVS_SendEvent(MQ2SB_REMOVE_ALL_PKTS_ERROR_EID, CFE_EVS_INFORMATION,
                        "Attempted to remove %d packet entries. Failed %d unsubscribes",
                        PktCnt, FailedUnsubscribe);
   }

   return RetStatus;

} /* End of MQ2SB_RemoveAllPktsCmd() */


/******************************************************************************
** Function:  MQ2SB_InitStats
**
** If TxOutputInterval==0 then retain current stats
** ComputeStats() logic assumes at least 1 init cycle
**
*/
void MQ2SB_InitStats(uint16 TxOutputInterval, uint16 InitDelay)
{
   
   if (TxOutputInterval != 0) Mq2Sb->Stats.TxOutputInterval = (double)TxOutputInterval;

   Mq2Sb->Stats.State = MQ2SB_STATS_INIT_CYCLE;
   Mq2Sb->Stats.InitCycles = (Mq2Sb->Stats.TxOutputInterval >= InitDelay) ? 1 : (double)InitDelay/Mq2Sb->Stats.TxOutputInterval;
            
   Mq2Sb->Stats.IntervalMilliSecs = 0.0;
   Mq2Sb->Stats.TxIntervalPkts = 0;
   Mq2Sb->Stats.TxIntervalBytes = 0;
      
   Mq2Sb->Stats.TxPrevIntervalAvgPkts  = 0.0;
   Mq2Sb->Stats.TxPrevIntervalAvgBytes = 0.0;
   
   Mq2Sb->Stats.TxAvgPktsPerSec  = 0.0;
   Mq2Sb->Stats.TxAvgBytesPerSec = 0.0;

} /* End MQ2SB_InitStats() */