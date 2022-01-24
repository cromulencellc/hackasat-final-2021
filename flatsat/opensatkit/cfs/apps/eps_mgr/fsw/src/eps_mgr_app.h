/* 
** Purpose: Define a Eps_mgr application.
**
** Notes:
**   None
**
*/
#ifndef _eps_mgr_app_
#define _eps_mgr_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "eps_impl.h"
#include "sys/time.h"

/*
** Macro Definitions
*/

#define EPS_MGR_INIT_INFO_EID            (EPS_MGR_BASE_EID + 0)
#define EPS_MGR_EXIT_ERR_EID             (EPS_MGR_BASE_EID + 1)
#define EPS_MGR_CMD_NOOP_INFO_EID        (EPS_MGR_BASE_EID + 2)
#define EPS_MGR_CMD_INVALID_MID_ERR_EID  (EPS_MGR_BASE_EID + 3)
#define EPS_MGR_DEBUG_EID                (EPS_MGR_BASE_EID + 4)

/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class  CmdMgr;
   TBLMGR_Class  TblMgr;

   CFE_SB_PipeId_t CmdPipe;
   uint16  ExecuteCycleDelay;      /* Delays between execution cycles. This is controller update rate in msec*/
   
   EPS_IMPL_Class EpsImpl;
   struct timespec LoopStart;
   struct timespec LoopEnd;

} EPS_MGR_Class;

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
   uint16   EpsImplExeCnt;

} OS_PACK EPS_MGR_HkPkt;

#define EPS_MGR_TLM_HK_LEN sizeof (EPS_MGR_HkPkt)

/*
** Exported Data
*/

extern EPS_MGR_Class  Eps_mgr;

/*
** Exported Functions
*/

/******************************************************************************
** Function: EPS_MGR_AppMain
**
*/
void EPS_MGR_AppMain(void);

#endif /* _eps_mgr_app_ */
