/*
** Purpose: TODO
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
**
*/
#ifndef _sb2mq_
#define _sb2mq_

/*
** Includes
*/

#include "app_cfg.h"
#include "mqtt_client.h"

/***********************/
/** Macro Definitions **/
/***********************/
#define SB2MQ_CREATE_FILE_ERR_EID               (SB2MQ_BASE_EID + 0)
#define SB2MQ_LOAD_TYPE_ERR_EID                 (SB2MQ_BASE_EID + 1)
#define SB2MQ_LOAD_EMPTY_ERR_EID                (SB2MQ_BASE_EID + 2)
#define SB2MQ_LOAD_UPDATE_ERR_EID               (SB2MQ_BASE_EID + 3)
#define SB2MQ_LOAD_OPEN_ERR_EID                 (SB2MQ_BASE_EID + 4)
#define SB2MQ_LOAD_PKT_ATTR_ERR_EID             (SB2MQ_BASE_EID + 5)
#define SB2MQ_LOAD_UNDEF_FILTERS_EID            (SB2MQ_BASE_EID + 6)
#define SB2MQ_DEBUG_EID                         (SB2MQ_BASE_EID + 7)
#define SB2MQ_INIT_DEBUG_EID                    (SB2MQ_BASE_EID + 8)
#define SB2MQ_LOAD_TBL_SUBSCRIBE_ERR_EID        (SB2MQ_BASE_EID +  9)
#define SB2MQ_LOAD_TBL_INFO_EID                 (SB2MQ_BASE_EID +  10)
#define SB2MQ_LOAD_TBL_ERR_EID                  (SB2MQ_BASE_EID +  11)
#define SB2MQ_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID  (SB2MQ_BASE_EID +  12)
#define SB2MQ_LOAD_TBL_ENTRY_INFO_EID           (SB2MQ_BASE_EID +  13)
#define SB2MQ_PUBLISH_STATUS_EID                (SB2MQ_BASE_EID +  14)
#define SB2MQ_PUBLISH_STATUS_ERROR_EID          (SB2MQ_BASE_EID +  15)
#define SB2MQ_ADD_PKT_SUCCESS_EID               (SB2MQ_BASE_EID +  16)
#define SB2MQ_ADD_PKT_ERROR_EID                 (SB2MQ_BASE_EID + 17)
#define SB2MQ_REMOVE_PKT_SUCCESS_EID            (SB2MQ_BASE_EID + 18)
#define SB2MQ_REMOVE_PKT_ERROR_EID              (SB2MQ_BASE_EID + 19)
#define SB2MQ_REMOVE_ALL_PKTS_SUCCESS_EID       (SB2MQ_BASE_EID + 20)
#define SB2MQ_REMOVE_ALL_PKTS_ERROR_EID         (SB2MQ_BASE_EID + 21)
/* TODO #define SB2MQ_TBL_REGISTER_ERR_EID  (SB2MQ_BASE_EID + 0) */

#define SB2MQ_MAX_APP_ID    (0x19FF)  /* Maximum CCSDS v1 ApId */
#define SB2MQ_APP_ID_MASK   (0x19FF)  /* CCSDS v1 ApId mask    */
#define SB2MQ_UNUSED_MSG_ID (CFE_SB_INVALID_MSG_ID)
#define SB2MQ_INIT_EVS_TYPE CFE_EVS_DEBUG

/******************************************************************************
** Telemetry Packets
*/

// typedef struct {

//    uint8    Header[CFE_SB_TLM_HDR_SIZE];

//    CFE_SB_MsgId_t      StreamId;
//    CFE_SB_Qos_t        Qos;
//    uint16              BufferLimit;
//    uint16              MqttQos;
//    uint16              MqttRetained;
//    uint16              State;      /**< Message ID is enabled = 1        */
//    char                MqttTopic[MQTT_TOPIC_LEN]; /* MQTT Topic */ 
//    // uint16               FilterType;
//    // PktUtil_FilterParam  FilterParam;

// } OS_PACK SB2MQ_PktTlm;

// #define SB2MQ_PKT_TLM_LEN sizeof (SB2MQ_PktTlm)

/**********************/
/** Type Definitions **/
/**********************/

/*
** Table Structure Objects 
*/

#define  SB2MQ_OBJ_PKT       0
#define  SB2MQ_OBJ_FILTER    1
#define  SB2MQ_OBJ_CNT       2

#define  SB2MQ_OBJ_NAME_PKT     "packet"
#define  SB2MQ_OBJ_NAME_FILTER  "filter"


// typedef struct {

//    uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
//    CFE_SB_MsgId_t     StreamId;

// }  SB2MQ_SendPktTblTlmCmdParam;
// #define SB2MQ_SEND_PKT_TBL_TLM_CMD_DATA_LEN  (sizeof(SB2MQ_SendPktTblTlmCmdParam) - CFE_SB_CMD_HDR_SIZE)



/*
** Table
*/

typedef struct {
   
   CFE_SB_MsgId_t      StreamId;
   CFE_SB_Qos_t        Qos;
   uint16              BufLim;
   PktUtil_Filter      Filter;
   uint16              MqttQos;
   uint16              MqttRetained;
   uint16              MqttIncludeCcsdsHeader;
   char                MqttTopic[MQTT_TOPIC_LEN]; /* MQTT Topic */

} SB2MQ_TblEntry;


typedef struct {
   
   SB2MQ_TblEntry Pkt[SB2MQ_MAX_APP_ID];

} SB2MQ_Tbl;


/*
** Class 
*/

/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const SB2MQ_Tbl* (*SB2MQ_GetTblPtrFcn)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*SB2MQ_LoadTblFcn)(SB2MQ_Tbl* NewTbl); 

/* Table Owner's function to load a single table pkt. The JSON object/container is an array */
typedef boolean (*SB2MQ_LoadTblEntryFcn)(uint16 PktIdx, SB2MQ_TblEntry* NewPkt);

typedef enum {

   SB2MQ_STATS_INIT_CYCLE    = 1,
   SB2MQ_STATS_INIT_INTERVAL,
   SB2MQ_STATS_VALID
   
} SB2MQ_StatsState;

typedef struct {

   uint16  InitCycles;         /* 0: Init done, >0: Number of remaining init cycles  */  
   
   double  TxOutputInterval;   /* ms between calls to SB2MQ_OutputTelemetry()    */  
   double  IntervalMilliSecs;  /* Number of ms in the current computational cycle */
   uint32  TxIntervalPkts;
   uint32  TxIntervalBytes;
   
   CFE_TIME_SysTime_t PrevTime; 
   double  TxPrevIntervalAvgPkts;
   double  TxPrevIntervalAvgBytes;
   
   double  TxAvgPktsPerSec;
   double  TxAvgBytesPerSec;
   
   SB2MQ_StatsState State;
   
} SB2MQ_Stats;

typedef struct {

   /*
   ** App Framework
   */
   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   PktLoadCnt;
   uint16   FilterLoadCnt;
   uint16   CurAppId;   /* Most recent AppId processed by the callback function */
   
   /*
   ** Tables
   */
   SB2MQ_Tbl Tbl;

   /*
   ** SB Pipe
   */
   CFE_SB_PipeId_t    TxPipe;

   /**
    * Telemtry Packets
    */
   // SB2MQ_PktTlm   PktTlm;


   // /* Other State Definitions */
   boolean           OutputEnabled;
   boolean           SuppressSend;
   SB2MQ_Stats       Stats;

   SB2MQ_GetTblPtrFcn    GetTblPtrFunc;
   SB2MQ_LoadTblFcn      LoadTblFunc;
   SB2MQ_LoadTblEntryFcn LoadTblEntryFunc; 

   JSON_Class Json;
   JSON_Obj   JsonObj[SB2MQ_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];


} SB2MQ_Class;


typedef struct {

   uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
   SB2MQ_TblEntry      NewPkt;

}  OS_PACK SB2MQ_AddPktCmdParam;
#define SB2MQ_ADD_PKT_CMD_DATA_LEN  (sizeof(SB2MQ_AddPktCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t     StreamId;

}  SB2MQ_RemovePktCmdParam;
#define SB2MQ_REMOVE_PKT_CMD_DATA_LEN  (sizeof(SB2MQ_RemovePktCmdParam) - CFE_SB_CMD_HDR_SIZE)

/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: SB2MQ_Constructor
**
** Initialize the example cFE Table object.
**
** Notes:
**   None
*/
void SB2MQ_Constructor(SB2MQ_Class*  Sb2MqPtr,
                       MQTT_CLIENT_Class* Sb2MqClientPtr,
                       uint16* MqttAppRunLoopMsPtr,
                       SB2MQ_GetTblPtrFcn    GetTblPtrFunc,
                       SB2MQ_LoadTblFcn      LoadTblFunc, 
                       SB2MQ_LoadTblEntryFcn LoadTblEntryFunc,
                       const char* PipeName, 
                       uint16 PipeDepth);

/******************************************************************************
** Function: SB2MQ_OutputPackets
**
*/
uint16 SB2MQ_OutputPackets(void);

/******************************************************************************
** Function:  SB2MQ_InitStats
**
** OutputTlmInterval - Number of ms between calls to SB2MQ_OutputTelemetry()
**                     If zero retain the last interval value
** InitDelay         - Number of ms to delay starting stats computation
*/
void SB2MQ_InitStats(uint16 OutputTlmInterval, uint16 InitDelay);

/******************************************************************************
** Function: KTTBL_SetPacketToUnused
**
*/
void SB2MQ_SetPacketToUnused(SB2MQ_TblEntry* PktPtr);


/******************************************************************************
** Function: SB2MQ_LoadUnusedPacketArray
**
*/
void SB2MQ_SetTblToUnused(SB2MQ_Tbl* TblPtr);


/******************************************************************************
** Function: SB2MQ_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavour of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void SB2MQ_ResetStatus(void);

/******************************************************************************
** Function: SB2MQ_GetTblPtr
**
** Return a pointer to the packet table.
**
** Notes:
**   1. Function signature must match SB2MQ_GetTblPtr
**
*/
const SB2MQ_Tbl* SB2MQ_GetTblPtr(void);

/******************************************************************************
** Function: SB2MQ_LoadTbl
**
** Unsubscribe from all current SB telemetry, flushes the telemetry input pipe,
** loads the entire new table and subscribes to each message.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match SBNTBL_LoadTbl
**
*/
boolean SB2MQ_LoadTbl(SB2MQ_Tbl* NewTbl);

/******************************************************************************
** Function: SB2MQ_LoadTblEntry
**
** Load a single message table entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**   2. Function signature must match SB2MQ_LoadTblEntry
**
*/
boolean SB2MQ_LoadTblEntry(uint16 PktIdx, SB2MQ_TblEntry* PktArray);

/******************************************************************************
** Function: SB2MQ_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean SB2MQ_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: SB2MQ_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean SB2MQ_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);


/******************************************************************************
** Function: SB2MQ_ManageTbl
**
** Manage the cFE table interface for table loads and validation. 
*/
// boolean SB2MQ_ManageTbl(void);


/******************************************************************************
** Function: SubscribeNewPkt
**
** Subscribe to SB message packet from table data entry
*/
int32 SB2MQ_SubscribeNewPkt(SB2MQ_TblEntry* NewPkt);

/******************************************************************************
** Function: SB2MQ_AddPktCmd
**
** Add a packet to the table and subscribe for it on the SB.
**
** Notes:
**   1. Command rejected if table has existing entry for thecommanded Stream ID
**   2. Only update the table if the software bus subscription successful
** 
*/
boolean SB2MQ_AddPktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SB2MQ_RemoveAllPktsCmd
**
** Notes:
**   1. The cFE to_lab code unsubscribes the command and send HK MIDs. I'm not
**      sure why this is done and I'm not sure how the command is used. This 
**      command is intended to help manage TO telemetry packets.
*/
boolean SB2MQ_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SB2MQ_RemovePktCmd
**
** Remove a packet from the table and unsubscribe from receiving it on the SB.
**
** Notes:
**   1. Don't consider trying to remove an non-existent entry an error
*/
boolean SB2MQ_RemovePktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SB2MQ_SendPktTblTlmCmd
**
** Send a telemetry packet containg the packet table entry for the commanded
** Stream ID.
**
*/
boolean SB2MQ_SendPktTblTlmCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SB2MQ_UpdateFilterCmd
**
** Notes:
**   1. Command rejected if AppId packet entry has not been loaded 
**   2. The filter type is verified but the filter parameter values are not 
** 
*/
boolean SB2MQ_UpdateFilterCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _sb2mq_ */
