/* 
** Purpose: Define a Adcs application.
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
#ifndef _adcs_app_
#define _adcs_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "adcs_conf_tbl.h"
#include "adcs_impl.h"

/*
** Macro Definitions
*/

#define ADCS_INIT_INFO_EID            (ADCS_BASE_EID + 0)
#define ADCS_EXIT_ERR_EID             (ADCS_BASE_EID + 1)
#define ADCS_CMD_NOOP_INFO_EID        (ADCS_BASE_EID + 2)
#define ADCS_CMD_INVALID_MID_ERR_EID  (ADCS_BASE_EID + 3)
#define ADCS_APP_DEBUG_EID            (ADCS_BASE_EID + 4)
#define ADCS_SET_EXE_DEBUG_EID        (ADCS_BASE_EID + 5)


/*
** Type Definitions
*/
typedef struct {
    uint8   CmdHeader[CFE_SB_CMD_HDR_SIZE];
    uint16_t ExecutionDelayMsec;
}  OS_PACK ADCS_SetExecutionDelayCmdPkt;
#define ADCS_SET_EXECUTION_DELAY_CMD_DATA_LEN  (sizeof(ADCS_SetExecutionDelayCmdPkt) - CFE_SB_CMD_HDR_SIZE)

typedef struct
{
   CFE_SB_PipeId_t SbPipe;
   CMDMGR_Class  CmdMgr;
   TBLMGR_Class  TblMgr;

   // ADCS APP
   uint16  ExecuteCycleDelay;      /* Delays between execution cycles. This is controller update rate in msec*/
   ADCS_IMPL_Class AdcsImpl;
   struct timespec LoopStart;
   struct timespec LoopEnd;

} ADCS_Class;

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
   ** ADCS Control Data
   */
   uint32   CtrlExeCnt;
   uint16   CtrlMode;
   uint16   CtrlExeDelay;

   /*
   ** HW Manager Data
   */
   uint32 HwMgrExeCnt;
   
} OS_PACK ADCS_HkPkt;

#define ADCS_TLM_HK_LEN sizeof (ADCS_HkPkt)


// #define ADCS_SENSOR_DATA_PKT_LEN sizeof (ADCS_HkPkt)
// #define MQTT_MSG_STR4000_TLM_PKT_LEN  sizeof(MQTT_MSG_Str4000TlmPkt)

/*
** Exported Data
*/

extern ADCS_Class  Adcs;

/*
** Exported Functions
*/

/******************************************************************************
** Function: ADCS_AppMain
**
*/
void ADCS_AppMain(void);


#endif /* _adcs_app_ */
