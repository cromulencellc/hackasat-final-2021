/*
** Purpose: Define a communication object.
**
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
*/

#ifndef _comm_obj_
#define _comm_obj_

/*
** Includes
*/

#include "app_cfg.h"
// #include "trans_udp.h"
#include "network_includes.h"

/*
** Event Message IDs
*/

#define COMM_OBJ_CMD_ENA_INFO_EID  (COMM_OBJ_BASE_EID + 0)
#define COMM_OBJ_DEMO_CMD_INFO_EID (COMM_OBJ_BASE_EID + 1)
#define COMM_OBJ_DEMO_DEBUG_EID    (COMM_OBJ_BASE_EID + 2)

/*
 ** Message msg & sync definitions
 */
#define COMM_DMD_PKT_MSG_ID    0x444D
#define COMM_MOD_PKT_MSG_ID    0x4D4F
#define COMM_KEY_PKT_MSG_SYNC  0x41414141
#define COMM_PING_PKT_MSG_SYNC 0x42424242

/*
** Type Definitions
*/

typedef struct
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];
   char     Synch[4];
   float    frequency;
   float    rf_bandwidth;
   float    sampling_frequency;
   uint8    gain_control_mode;
   uint8    Pad[3];
   float    hardwaregain;
   float    rssi;
   float    ss_error;
   float    costas_error;
   float    sync_state;
   float    frames_received;
   float    frames_decoded;
   float    powerdown;
   float    temp;

} OS_ALIGN(4) COMM_OBJ_DemodPkt;
#define COMM_OBJ_DEMOD_LEN sizeof (COMM_OBJ_DemodPkt)

typedef struct
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];
   char     Synch[4];
   float    frequency;
   float    rf_bandwidth;
   float    sampling_frequency;
   float    hardwaregain;
   float    rssi;
   float    pkts_received;
   float    powerdown;
   float    temp;
} OS_ALIGN(4) COMM_OBJ_ModPkt;
#define COMM_OBJ_MOD_LEN sizeof (COMM_OBJ_ModPkt)

typedef struct
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];
   uint32   Synch;
   uint64   AttrKey;
} OS_PACK COMM_OBJ_KeyPkt;
#define COMM_OBJ_KeyPkt_LEN sizeof (COMM_OBJ_KeyPkt)

typedef struct
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];
   uint32   AttrKey1;
   uint32   AttrKey2;
} OS_PACK COMM_OBJ_AttrUpdatePkt;
#define COMM_OBJ_AttrUpdatePkt_LEN sizeof (COMM_OBJ_AttrUpdatePkt)

typedef struct 
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];
   uint32   Synch;
   uint64   Status;
} OS_PACK COMM_OBJ_PingPkt;
#define COMM_OBJ_PingPkt_LEN sizeof (COMM_OBJ_PingPkt)

typedef struct 
{
   uint8    Header[CFE_SB_TLM_HDR_SIZE];
   uint64   PingStatus;
} OS_PACK COMM_OBJ_PingStatus;
#define COMM_OBJ_PingStatus_LEN sizeof (COMM_OBJ_PingStatus)


/******************************************************************************
** CommObj_Class
*/

typedef struct {

   CFE_SB_PipeId_t   TlmPipe;

   uint16       ExecCnt;

   COMM_OBJ_DemodPkt   DemodPkt;
   COMM_OBJ_ModPkt     ModPkt;
   COMM_OBJ_KeyPkt     KeyPkt;
   COMM_OBJ_AttrUpdatePkt AttrPkt;
   COMM_OBJ_PingPkt    PingPkt;
   COMM_OBJ_PingStatus StatusPkt;

   uint64       SLA_Attribution_Key;
   uint64       SLA_Ping_Status;

   char         CommStatusString[COMM_MAX_STATUS_STRING_SIZE];

} COMM_OBJ_Class;


/******************************************************************************
** Command Functions
*/

typedef struct
{

   uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16  Parameter;

} COMM_OBJ_DemoCmdMsg;
#define COMM_OBJ_DEMO_CMD_DATA_LEN  (sizeof(COMM_OBJ_DemoCmdMsg) - CFE_SB_CMD_HDR_SIZE)

/*
** Exported Functions
*/

/******************************************************************************
** Function: COMM_OBJ_Constructor
**
** Initialize the example object to a known state
**
** Notes:
**   1. This must be called prior to any other function.
**   2. The table values are not populated. This is done when the table is 
**      registered with the table manager.
**
*/
void COMM_OBJ_Constructor(COMM_OBJ_Class *CommObjPtr);


/******************************************************************************
** Function: COMM_OBJ_Execute
**
** Execute main object function.
**
*/
void COMM_OBJ_Execute(void);


/******************************************************************************
** Function: COMM_OBJ_ResetStatus
**
** Reset counters and status flags to a known reset state.
**
** Notes:
**   1. Any counter or variable that is reported in HK telemetry that doesn't
**      change the functional behavior should be reset.
**
*/
void COMM_OBJ_ResetStatus(void);

/******************************************************************************
** Function: COMM_OBJ_DemoCmd
**
** Demonstrate an 'entity' object having a command.
**
** Note:
**  1. This function must comply with the CMDMGR_CmdFuncPtr definition
*/
boolean COMM_OBJ_DemoCmd(void* DataObjPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _comm_obj_ */
