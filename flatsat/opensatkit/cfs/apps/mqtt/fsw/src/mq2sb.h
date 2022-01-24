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
#ifndef _mq2sb_
#define _mq2sb_

/*
** Includes
*/

#include "app_cfg.h"
#include "mqtt_msg.h"
#include "mqtt_client.h"

/***********************/
/** Macro Definitions **/
/***********************/

#define MQ2SB_CREATE_FILE_ERR_EID               (MQ2SB_BASE_EID + 0)
#define MQ2SB_LOAD_TYPE_ERR_EID                 (MQ2SB_BASE_EID + 1)
#define MQ2SB_LOAD_EMPTY_ERR_EID                (MQ2SB_BASE_EID + 2)
#define MQ2SB_LOAD_UPDATE_ERR_EID               (MQ2SB_BASE_EID + 3)
#define MQ2SB_LOAD_OPEN_ERR_EID                 (MQ2SB_BASE_EID + 4)
#define MQ2SB_LOAD_PKT_ATTR_ERR_EID             (MQ2SB_BASE_EID + 5)
#define MQ2SB_LOAD_UNDEF_FILTERS_EID            (MQ2SB_BASE_EID + 6)
#define MQ2SB_DEBUG_EID                         (MQ2SB_BASE_EID + 7)
#define MQ2SB_INIT_DEBUG_EID                    (MQ2SB_BASE_EID + 8)
#define MQ2SB_LOAD_TBL_SUBSCRIBE_ERR_EID        (MQ2SB_BASE_EID +  9)
#define MQ2SB_LOAD_TBL_INFO_EID                 (MQ2SB_BASE_EID +  10)
#define MQ2SB_LOAD_TBL_ERR_EID                  (MQ2SB_BASE_EID +  11)
#define MQ2SB_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID  (MQ2SB_BASE_EID +  12)
#define MQ2SB_LOAD_TBL_ENTRY_INFO_EID           (MQ2SB_BASE_EID +  13)
#define MQ2SB_PUBLISH_STATUS_EID                (MQ2SB_BASE_EID +  14)
#define MQ2SB_CLIENT_SUBSCRIBE_EID              (MQ2SB_BASE_EID + 15)
#define MQSSB_CLIENT_SUBSCRIBE_ERR_EID          (MQ2SB_BASE_EID + 16)
#define MQ2SB_REMOVE_PKT_SUCCESS_EID            (MQ2SB_BASE_EID + 17)
#define MQ2SB_REMOVE_PKT_ERROR_EID              (MQ2SB_BASE_EID + 18)
#define MQ2SB_REMOVE_ALL_PKTS_SUCCESS_EID       (MQ2SB_BASE_EID + 19)
#define MQ2SB_REMOVE_ALL_PKTS_ERROR_EID         (MQ2SB_BASE_EID + 20)

#define MQ2SB_UNUSED_MSG_ID (CFE_SB_INVALID_MSG_ID)
#define MQ2SB_APP_ID_MASK   (0x19FF)  /* CCSDS v1 ApId mask    */
#define MQ2SB_INIT_EVS_TYPE CFE_EVS_DEBUG

/**********************/
/** Type Definitions **/
/**********************/
/*
** Table Structure Objects 
*/

#define  MQ2SB_OBJ_PKT       0
#define  MQ2SB_OBJ_CNT       1

#define  MQ2SB_OBJ_NAME_PKT     "message"


/******************************************************************************
** Table Struture
*/

typedef struct {
   
   uint16              MsgType;       /* MQTT_MSG_TYPE_* */
   CFE_SB_MsgId_t      MsgId;         /* Message ID (must be unique)  */
   uint16              DataType;      /* MQTT Message Data Type  */
   uint16              DataLength;    /* MQTT Message Data Length */
   uint16              MqttQos;       /* MQTT MSG Sub QOS */
   char                MqttTopic[MQTT_TOPIC_LEN]; /* MQTT Topic */
} MQ2SB_TblEntry;



typedef struct {
   
   MQ2SB_TblEntry Entry[MQTT_MQ2SB_TBL_ENTRIES];

} MQ2SB_Tbl;


/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const MQ2SB_Tbl* (*MQ2SB_GetTblPtrFcn)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*MQ2SB_LoadTblFcn)(MQ2SB_Tbl* NewTbl); 

/* Table Owner's function to load a single table pkt. The JSON object/container is an array */
typedef boolean (*MQ2SB_LoadTblEntryFcn)(uint16 PktIdx, MQ2SB_TblEntry* NewPkt);

/******************************************************************************
** Command Definitions
*/

typedef struct {

   uint8               CmdHeader[CFE_SB_CMD_HDR_SIZE];
   MQ2SB_TblEntry      NewPtk;

}  OS_PACK MQ2SB_AddPktCmdParam;
#define MQ2SB_ADD_PKT_CMD_DATA_LEN  (sizeof(MQ2SB_AddPktCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char               MqttTopic[MQTT_TOPIC_LEN]; /* MQTT Topic */

}  MQ2SB_RemovePktCmdParam;
#define MQ2SB_REMOVE_PKT_CMD_DATA_LEN  (sizeof(MQ2SB_RemovePktCmdParam) - CFE_SB_CMD_HDR_SIZE)

/******************************************************************************
** Class Struture
*/


typedef enum {

   MQ2SB_STATS_INIT_CYCLE    = 1,
   MQ2SB_STATS_INIT_INTERVAL,
   MQ2SB_STATS_VALID
   
} MQ2SB_StatsState;

typedef struct {

   uint16  InitCycles;         /* 0: Init done, >0: Number of remaining init cycles  */  
   
   double  TxOutputInterval;   /* ms between calls to MQ2SB_OutputTelemetry()    */  
   double  IntervalMilliSecs;  /* Number of ms in the current computational cycle */
   uint32  TxIntervalPkts;
   uint32  TxIntervalBytes;
   
   CFE_TIME_SysTime_t PrevTime; 
   double  TxPrevIntervalAvgPkts;
   double  TxPrevIntervalAvgBytes;
   
   double  TxAvgPktsPerSec;
   double  TxAvgBytesPerSec;
   
   MQ2SB_StatsState State;
   
} MQ2SB_Stats;

typedef struct {

   /*
   ** App Framework
   */
   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   EntryLoadCnt;
   uint16   CurMsgId; /* Most recent AppId processed by the callback function */
   uint16   TblMsgCount;

   boolean           OutputEnabled;
   boolean           SuppressSend;
   MQ2SB_Stats       Stats;

   MQ2SB_GetTblPtrFcn    GetTblPtrFunc;
   MQ2SB_LoadTblFcn      LoadTblFunc;
   MQ2SB_LoadTblEntryFcn LoadTblEntryFunc; 
   
   MQ2SB_Tbl               Tbl;
   MQTT_MSG_SbPkt          SbPkt;

   /*
   ** Tables
   */
   JSON_Class Json;
   JSON_Obj   JsonObj[MQ2SB_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} MQ2SB_Class;

/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: MQ2SB_Constructor
**
** Initialize the example cFE Table object.
**
** Notes:
**   None
*/
void MQ2SB_Constructor(MQ2SB_Class*  Mq2SbPtr,
                       MQTT_CLIENT_Class* Mq2SbClientPtr,
                       uint16* MqttAppRunLoopMsPtr,
                       MQ2SB_GetTblPtrFcn    GetTblPtrFunc,
                       MQ2SB_LoadTblFcn      LoadTblFunc, 
                       MQ2SB_LoadTblEntryFcn LoadTblEntryFunc);


/******************************************************************************
** Function: MQ2SB_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavour of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void MQ2SB_ResetStatus(void);


/******************************************************************************
** Function:  MQ2SB_InitStats
**
** OutputTlmInterval - Number of ms between calls to MQ2SB_OutputTelemetry()
**                     If zero retain the last interval value
** InitDelay         - Number of ms to delay starting stats computation
*/
void MQ2SB_InitStats(uint16 OutputTlmInterval, uint16 InitDelay);

/******************************************************************************
** Function: KTTBL_SetPacketToUnused
**
*/
void MQ2SB_SetEntryToUnused(MQ2SB_TblEntry* PktPtr);


/******************************************************************************
** Function: MQ2SB_GetTblPtr
**
** Return a pointer to the sb-pkt table.
**
** Notes:
**   1. Function signature must match MQ2SB_GetTblPtr
**
*/
const MQ2SB_Tbl* MQ2SB_GetTblPtr(void);

/******************************************************************************
** Function: MQ2SB_LoadTbl
**
** Unsubscribe from all current SB telemetry, flushes the telemetry input pipe,
** loads the entire new table and subscribes to each message.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match SBNTBL_LoadTbl
**
*/
boolean MQ2SB_LoadTbl(MQ2SB_Tbl* NewTbl);

/******************************************************************************
** Function: MQ2SB_LoadTblEntry
**
** Load a single message table entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**   2. Function signature must match MQ2SB_LoadTblEntry
**
*/
boolean MQ2SB_LoadTblEntry(uint16 PktIdx, MQ2SB_TblEntry* PktArray);

/******************************************************************************
** Function: MQ2SB_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean MQ2SB_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: MQ2SB_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean MQ2SB_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);


/******************************************************************************
** Function: MQ2SB_LoadUnusedPacketArray
**
*/
void MQ2SB_SetTblToUnused(MQ2SB_Tbl* TblPtr);


/******************************************************************************
** Function: MQ2SBT_ClientSubscribe
**/
void MQ2SB_ClientSubscribe(void);



/******************************************************************************
** Function: MQ2SB_RemoveAllPktsCmd
**/
boolean MQ2SB_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

#endif /* _mq2sb_ */
