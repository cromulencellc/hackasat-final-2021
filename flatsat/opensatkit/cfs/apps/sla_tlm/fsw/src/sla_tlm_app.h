/* 
** Purpose: Define a Sla_tlm application.
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/
#ifndef _sla_tlm_app_
#define _sla_tlm_app_

/*
** Includes
*/

#include "app_cfg.h"

/*
** Macro Definitions
*/

#define SLA_TLM_INIT_INFO_EID            (SLA_TLM_BASE_EID + 0)
#define SLA_TLM_EXIT_ERR_EID             (SLA_TLM_BASE_EID + 1)
#define SLA_TLM_CMD_NOOP_INFO_EID        (SLA_TLM_BASE_EID + 2)
#define SLA_TLM_CMD_INVALID_MID_ERR_EID  (SLA_TLM_BASE_EID + 3)

/*
** Type Definitions
*/

typedef struct
{

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** Example Table Data 
   ** - Loaded with status from the last table action 
   */

   uint8    LastAction;
   uint8    LastActionStatus;
   
   /*
   ** EXOBJ Data
   */

   uint16   ExObjExecCnt;

  /*
   ** Saved data from recieved messages
   */

   uint64    Key;
   uint8     RoundNum;
   uint16    SequenceNum;
   uint64    PingStatus;
   uint64    CommTelemField1;
   uint64    CommTelemField2;
   uint64    CommTelemField3;
   uint64    CommTelemField4;
   uint64    CommTelemField5;
   uint64    CommTelemField6;
   uint64    CommTelemField7;
   uint64    CommTelemField8;

} OS_PACK SLA_TLM_HkPkt;
#define SLA_TLM_TLM_HK_LEN sizeof (SLA_TLM_HkPkt)

typedef struct
{
   uint8     Header[CFE_SB_TLM_HDR_SIZE];
   uint64    Key;
} OS_PACK SLA_TLM_AttrKeyPkt;
#define SLA_TLM_AttrKeyPkt_LEN sizeof (SLA_TLM_AttrKeyPkt)

typedef struct 
{
   uint8     Header[CFE_SB_TLM_HDR_SIZE];
   uint32    Synch;
   uint64    Status;
} OS_PACK SLA_TLM_PingPkt;
#define SLA_TLM_PingPkt_LEN sizeof (SLA_TLM_PingPkt)

typedef struct 
{
   uint8     Header[CFE_SB_TLM_HDR_SIZE];
   uint64    Status;
} OS_PACK SLA_TLM_CommPingStatus;
#define SLA_TLM_CommPingStatus_LEN sizeof (SLA_TLM_CommPingStatus)

typedef struct
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];
   uint64   field1;
   uint64   field2;
   uint64   field3;
   uint64   field4;
   uint64   field5;
   uint64   field6;
   uint64   field7;
   uint64   field8;
} OS_PACK SLA_COMM_Telem_Pkt;
#define SLA_COMM_Telem_Pkt_LEN sizeof (SLA_COMM_Telem_Pkt)


typedef struct
{

   CMDMGR_Class    CmdMgr;
   CFE_SB_PipeId_t CmdPipe;
   uint64          AttributionKey;
   uint8           RoundNum;
   uint16          SequenceNum;
   uint64          PingStatus;
   uint64          CommTelemField1;
   uint64          CommTelemField2;
   uint64          CommTelemField3;
   uint64          CommTelemField4;
   uint64          CommTelemField5;
   uint64          CommTelemField6;
   uint64          CommTelemField7;
   uint64          CommTelemField8;

} SLA_TLM_Class;


/*
** Exported Data
*/

extern SLA_TLM_Class  Sla_tlm;

/*
** Exported Functions
*/

/******************************************************************************
** Function: SLA_TLM_AppMain
**
*/
void SLA_TLM_AppMain(void);

#endif /* _sla_tlm_app_ */
