/* 
** Purpose: Manage the Packet Table that defines which packets will be sent from the
**          software bus to a socket.
**
** Notes:
**   1. This has some of the features of a flight app such as packet filtering but it
**      would need design/code reviews to transition it to a flight mission. For starters
**      it uses UDP sockets and it doesn't regulate output bit rates. 
**
** License:
**   Written by David McComas, licensed under the copyleft GNU
**   General Public License (GPL). 
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _pktmgr_
#define _pktmgr_

/*
** Includes
*/

#include "app_cfg.h"
#include "sbntbl.h"
#include "network_includes.h"


#define SBNMGR_IP_STR_LEN  16
#define SBNMGR_OUTPUT_BUFFER_LEN 1024


/*
** Event Message IDs
*/
#define SBNMGR_SOCKET_SEND_ERR_EID               (SBNMGR_BASE_EID +  0)
#define SBNMGR_LOAD_TBL_SUBSCRIBE_ERR_EID        (SBNMGR_BASE_EID +  1)
#define SBNMGR_LOAD_TBL_INFO_EID                 (SBNMGR_BASE_EID +  2)
#define SBNMGR_LOAD_TBL_ERR_EID                  (SBNMGR_BASE_EID +  3)
#define SBNMGR_LOAD_TBL_ENTRY_SUBSCRIBE_ERR_EID  (SBNMGR_BASE_EID +  4)
#define SBNMGR_LOAD_TBL_ENTRY_INFO_EID           (SBNMGR_BASE_EID +  5)
#define SBNMGR_OUTPUT_ENA_INFO_EID               (SBNMGR_BASE_EID +  6)
#define SBNMGR_OUTPUT_ENA_SOCKET_ERR_EID         (SBNMGR_BASE_EID +  7)
#define SBNMGR_ADD_PKT_SUCCESS_EID               (SBNMGR_BASE_EID +  8)
#define SBNMGR_ADD_PKT_ERROR_EID                 (SBNMGR_BASE_EID +  9)
#define SBNMGR_REMOVE_PKT_SUCCESS_EID            (SBNMGR_BASE_EID + 10)
#define SBNMGR_REMOVE_PKT_ERROR_EID              (SBNMGR_BASE_EID + 11)
#define SBNMGR_REMOVE_ALL_PKTS_SUCCESS_EID       (SBNMGR_BASE_EID + 12)
#define SBNMGR_REMOVE_ALL_PKTS_ERROR_EID         (SBNMGR_BASE_EID + 13)
#define SBNMGR_DESTRUCTOR_INFO_EID               (SBNMGR_BASE_EID + 14)
#define SBNMGR_UPDATE_FILTER_CMD_SUCCESS_EID     (SBNMGR_BASE_EID + 15)
#define SBNMGR_UPDATE_FILTER_CMD_ERR_EID         (SBNMGR_BASE_EID + 16)
#define SBNMGR_DEBUG_EID                         (SBNMGR_BASE_EID + 17)
#define SBNMGR_RXSOCKET_CREATE_ERR_EID           (SBNMGR_BASE_EID + 18)
#define SBNMGR_RXSOCKET_BIND_ERR_EID             (SBNMGR_BASE_EID + 19)
#define SBNMGR_RECV_LEN_ERR_EID                  (SBNMGR_BASE_EID + 20)

/*
** Type Definitions
*/


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   char    DestIp[SBNMGR_IP_STR_LEN];

} SBNMGR_EnableOutputCmdParam;
#define PKKTMGR_ENABLE_OUTPUT_CMD_DATA_LEN  (sizeof(SBNMGR_EnableOutputCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8                CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t       StreamId;
   CFE_SB_Qos_t         Qos;
   uint8                BufLim;
   uint16               FilterType;
   PktUtil_FilterParam  FilterParam;

}  OS_PACK SBNMGR_AddPktCmdParam;
#define PKKTMGR_ADD_PKT_CMD_DATA_LEN  (sizeof(SBNMGR_AddPktCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t     StreamId;

}  SBNMGR_RemovePktCmdParam;
#define PKKTMGR_REMOVE_PKT_CMD_DATA_LEN  (sizeof(SBNMGR_RemovePktCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8              CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t     StreamId;

}  SBNMGR_SendPktTblTlmCmdParam;
#define PKKTMGR_SEND_PKT_TBL_TLM_CMD_DATA_LEN  (sizeof(SBNMGR_SendPktTblTlmCmdParam) - CFE_SB_CMD_HDR_SIZE)


typedef struct {

   uint8                CmdHeader[CFE_SB_CMD_HDR_SIZE];
   CFE_SB_MsgId_t       StreamId;
   uint16               FilterType;
   PktUtil_FilterParam  FilterParam;

}  OS_PACK SBNMGR_UpdateFilterCmdParam;
#define PKKTMGR_UPDATE_FILTER_CMD_DATA_LEN  (sizeof(SBNMGR_UpdateFilterCmdParam) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   CFE_SB_MsgId_t  StreamId;
   CFE_SB_Qos_t    Qos;
   uint16          BufLim;

   uint16               FilterType;
   PktUtil_FilterParam  FilterParam;

} OS_PACK SBNMGR_PktTlm;

#define SBNMGR_PKT_TLM_LEN sizeof (SBNMGR_PktTlm)


/******************************************************************************
** Packet Manager Class
*/

/*
** Packet Manager Statistics
** - Stats are computed over an interval of SBNMGR_COMPUTE_STATS_INTERVAL_MS
*/
typedef enum {

   SBNMGR_STATS_INIT_CYCLE    = 1,
   SBNMGR_STATS_INIT_INTERVAL,
   SBNMGR_STATS_VALID
   
} SBNMGR_StatsState;

typedef struct {

   uint16  InitCycles;         /* 0: Init done, >0: Number of remaining init cycles  */  
   
   double  TxOutputInterval;   /* ms between calls to SBNMGR_OutputTelemetry()    */  
   double  IntervalMilliSecs;  /* Number of ms in the current computational cycle */
   uint32  TxIntervalPkts;
   uint32  TxIntervalBytes;
   
   CFE_TIME_SysTime_t PrevTime; 
   double  TxPrevIntervalAvgPkts;
   double  TxPrevIntervalAvgBytes;
   
   double  TxAvgPktsPerSec;
   double  TxAvgBytesPerSec;
   
   SBNMGR_StatsState State;
   
} SBNMGR_Stats;


typedef struct {

   CFE_SB_PipeId_t   TxPipe;
   int               TxSockId;
   char              TxDestIp[SBNMGR_IP_STR_LEN];
   int               RxSockId;
   char              RxHostIp[SBNMGR_IP_STR_LEN];
   struct sockaddr_in RxSockAddr;


   uint8             outputBuffer[SBNMGR_OUTPUT_BUFFER_LEN];

   boolean           OutputEnable;
   boolean           SuppressSend;
   SBNMGR_Stats      Stats;

   SBNTBL_Tbl        Tbl;

   SBNMGR_PktTlm     PktTlm;

   boolean           RxBind;
   //Incorporate this into SBNMGR_Stats
   uint32            RxPktCnt;
   uint32            RxPktErrCnt;

   uint8             RecvBuff[SBNMGR_RECV_BUFF_LEN];
   CFE_SB_MsgPtr_t   RecvMsgPtr;

} SBNMGR_Class;

/*
** Exported Functions
*/

/******************************************************************************
** Function: SBNMGR_Constructor
**
** Construct a SBNMGR object. All table entries are cleared and the LoadTbl()
** function should be used to load an initial table.
**
** Notes:
**   1. This must be called prior to any other function.
**   2. Decoupling the initial table load gives an app flexibility in file
**      management during startup.
**
*/
void SBNMGR_Constructor(SBNMGR_Class *SbnMgrPtr, char* PipeName, uint16 PipeDepth);


/******************************************************************************
** Function:  SBNMGR_InitStats
**
** OutputTlmInterval - Number of ms between calls to SBNMGR_OutputTelemetry()
**                     If zero retain the last interval value
** InitDelay         - Number of ms to delay starting stats computation
*/
void SBNMGR_InitStats(uint16 OutputTlmInterval, uint16 InitDelay);


/******************************************************************************
** Function: SBNMGR_GetTblPtr
**
** Return a pointer to the packet table.
**
** Notes:
**   1. Function signature must match SBNTBL_GetTblPtr
**
*/
const SBNTBL_Tbl* SBNMGR_GetTblPtr(void);


/******************************************************************************
** Function: SBNMGR_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void SBNMGR_ResetStatus(void);


/******************************************************************************
** Function: SBNMGR_OutputPackets
**
** If output is enabled and output hasn't been suppressed it sends all of the
** SB packets on the input pipe out the socket.
**
*/
uint16 SBNMGR_OutputPackets(void);

/******************************************************************************
** Function: SBNMGR_ReadPackets
**
** Read up to MaxMsgRead messages and return the number of messages read.
**
*/
int SBNMGR_ReadPackets(uint16 MaxMsgRead);

/******************************************************************************
** Function: SBNMGR_LoadTbl
**
** Unsubscribe from all current SB telemetry, flushes the telemetry input pipe,
** loads the entire new table and subscribes to each message.
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match SBNTBL_LoadTbl
**
*/
boolean SBNMGR_LoadTbl(SBNTBL_Tbl* NewTbl);


/******************************************************************************
** Function: SBNMGR_LoadTblEntry
**
** Load a single message table entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**   2. Function signature must match SBNTBL_LoadTblEntry
**
*/
boolean SBNMGR_LoadTblEntry(uint16 PktIdx, SBNTBL_Pkt* PktArray);


/******************************************************************************
** Function: SBNMGR_EnableOutputCmd
**
** The commanded IP is always saved and downlink suppression is turned off. If
** downlink is disabled then a new socket is created with the new IP and
** downlink is turned on.
**
*/
// boolean SBNMGR_EnableOutputCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SBNMGR_AddPktCmd
**
** Add a packet to the table and subscribe for it on the SB.
**
** Notes:
**   1. Command rejected if table has existing entry for thecommanded Stream ID
**   2. Only update the table if the software bus subscription successful
** 
*/
boolean SBNMGR_AddPktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SBNMGR_RemoveAllPktsCmd
**
** Notes:
**   1. The cFE to_lab code unsubscribes the command and send HK MIDs. I'm not
**      sure why this is done and I'm not sure how the command is used. This 
**      command is intended to help manage TO telemetry packets.
*/
boolean SBNMGR_RemoveAllPktsCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SBNMGR_RemovePktCmd
**
** Remove a packet from the table and unsubscribe from receiving it on the SB.
**
** Notes:
**   1. Don't consider trying to remove an non-existent entry an error
*/
boolean SBNMGR_RemovePktCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SBNMGR_SendPktTblTlmCmd
**
** Send a telemetry packet containg the packet table entry for the commanded
** Stream ID.
**
*/
boolean SBNMGR_SendPktTblTlmCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SBNMGR_UpdateFilterCmd
**
** Notes:
**   1. Command rejected if AppId packet entry has not been loaded 
**   2. The filter type is verified but the filter parameter values are not 
** 
*/
boolean SBNMGR_UpdateFilterCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _pktmgr_ */
