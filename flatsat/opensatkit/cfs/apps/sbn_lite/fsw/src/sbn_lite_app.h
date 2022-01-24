/*
** Purpose: Define the SBN-lite application. This is a simple UDP-based
** single peer application for networking two instances of cfs. The packet
** table defines packets that are subscribed to on the SB and transmitted
** over UDP.  Any packet received on the UDP socket is validated and sent
** onto the SB for that processor. 
**
**
*/
#ifndef _sbn_lite_app_
#define _sbn_lite_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "sbntbl.h"
#include "sbnmgr.h"
#include "cf_msg.h"


/***********************/
/** Macro Definitions **/
/***********************/

/*
** Events
*/

#define SBN_LITE_APP_INIT_EID               (SBN_LITE_APP_BASE_EID + 0)
#define SBN_LITE_APP_NOOP_EID               (SBN_LITE_APP_BASE_EID + 1)
#define SBN_LITE_APP_EXIT_EID               (SBN_LITE_APP_BASE_EID + 2)
#define SBN_LITE_APP_INVALID_MID_EID        (SBN_LITE_APP_BASE_EID + 3)
#define SBN_LITE_SET_RUN_LOOP_DELAY_EID     (SBN_LITE_APP_BASE_EID + 4)
#define SBN_LITE_INVALID_RUN_LOOP_DELAY_EID (SBN_LITE_APP_BASE_EID + 5)
#define SBN_LITE_DEMO_EID                   (SBN_LITE_APP_BASE_EID + 6)
#define SBN_LITE_TEST_FILTER_EID            (SBN_LITE_APP_BASE_EID + 7)


/**********************/
/** Type Definitions **/
/**********************/


/******************************************************************************
** Command Packets
*/

typedef struct {

   uint8    CmdHeader[CFE_SB_CMD_HDR_SIZE];
   uint16   RunLoopDelay;

}  SBN_LITE_SetRunLoopDelayCmdParam;
#define SBN_LITE_SET_RUN_LOOP_DELAY_CMD_DATA_LEN  (sizeof(SBN_LITE_SetRunLoopDelayCmdParam) - CFE_SB_CMD_HDR_SIZE)


/******************************************************************************
** Telemetry Packets
*/
typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;


   uint16   RunLoopDelay;

   /*
   ** PKTTBL Data
   */

   uint8    PktTblLastLoadStatus;
   uint8    PktTblSpareAlignByte;
   uint16   PktTblAttrErrCnt;

   /*
   ** SBNMGR Data
   */

   uint8    StatsValid;
   uint8    SbnMgrSpareAlignByte;
   uint16   TxPktsPerSec;
   uint32   TxBytesPerSec;
   uint32   RxPktCnt;
   uint32   RxPktErrCnt;
   uint16   TxSockId;
   char     TxDestIp[SBNMGR_IP_STR_LEN];

} OS_PACK SBN_LITE_HkPkt;
#define SBN_LITE_TLM_HK_LEN sizeof (SBN_LITE_HkPkt)


/******************************************************************************
** SBN_LITE_Class
*/
typedef struct {

   /* 
   ** App Framework
   */   
   CFE_SB_PipeId_t CmdPipe;
   CMDMGR_Class CmdMgr;
   TBLMGR_Class TblMgr;

   /* 
   ** KIT_TO App 
   */   
   uint16 RunLoopDelay;

   /*
   ** Telemetry Packets
   */
   SBN_LITE_HkPkt     HkPkt;

   CF_PlaybackFileCmd_t CF_Playback_cmd;
   
   /*
   ** App Objects
   */ 
   SBNTBL_Class SbnTbl;
   SBNMGR_Class SbnMgr;

   uint32 ExecCnt;

} SBN_LITE_Class;


/*******************/
/** Exported Data **/
/*******************/

extern SBN_LITE_Class  SbnLite;


/************************/
/** Exported Functions **/
/************************/


/******************************************************************************
** Function: SBN_LITE_AppMain
**
*/
void SBN_LITE_AppMain(void);


/******************************************************************************
** Function: SBN_LITE_NoOpCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean SBN_LITE_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: SBN_LITE_ResetAppCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean SBN_LITE_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);

/******************************************************************************
** Function: SBN_LITE_SetRunLoopDelayCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean SBN_LITE_SetRunLoopDelayCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _sbn_lite_app_ */
