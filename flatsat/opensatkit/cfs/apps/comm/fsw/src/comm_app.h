/* 
** Purpose: Define a Comm application.
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
#ifndef _comm_app_
#define _comm_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "comm_obj.h"

/*
** Macro Definitions
*/

#define COMM_INIT_INFO_EID            (COMM_BASE_EID + 0)
#define COMM_EXIT_ERR_EID             (COMM_BASE_EID + 1)
#define COMM_CMD_NOOP_INFO_EID        (COMM_BASE_EID + 2)
#define COMM_CMD_INVALID_MID_ERR_EID  (COMM_BASE_EID + 3)

/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class   CmdMgr;
   COMM_OBJ_Class CommObj;
   
   CFE_SB_PipeId_t CmdPipe;

} COMM_Class;

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

   uint16   CommObjExecCnt;

   /**
    * Status message from comm
    * 
    */
   char     CommStatusString[COMM_MAX_STATUS_STRING_SIZE];

} OS_ALIGN(8) COMM_HkPkt;

#define COMM_TLM_HK_LEN sizeof (COMM_HkPkt)

/*
** Exported Data
*/

extern COMM_Class  Comm;

/*
** Exported Functions
*/

/******************************************************************************
** Function: COMM_AppMain
**
*/
void COMM_AppMain(void);

#endif /* _comm_app_ */
