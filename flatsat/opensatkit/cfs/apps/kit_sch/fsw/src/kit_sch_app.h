/*
** Purpose: Define the OpenSat Kit scheduler application. 
**
** Notes:
**   1. The scheduler object owns the message and scheduler tables
**      so it provides command functions. 
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
#ifndef _kit_sch_app_
#define _kit_sch_app_

/*
** Includes
*/

#include "app_cfg.h"
#include "schtbl.h"
#include "msgtbl.h"
#include "scheduler.h"

/*
** Macro Definitions
*/

#define KIT_SCH_NOOP_INFO_EID        (KIT_SCH_BASE_EID + 0)
#define KIT_SCH_INITSTATS_INF_EID    (KIT_SCH_BASE_EID + 1)
#define KIT_SCH_EXIT_EID             (KIT_SCH_BASE_EID + 2)
#define KIT_SCH_INVALID_MID_ERR_EID  (KIT_SCH_BASE_EID + 3)
#define KIT_SCH_DEBUG_EID            (KIT_SCH_BASE_EID + 4)
#define KIT_SCH_TOTAL_EID  5


/*
** Type Definitions
*/

typedef struct
{

   CMDMGR_Class CmdMgr;
   TBLMGR_Class TblMgr;
   
   SCHTBL_Class SchTbl;
   MSGTBL_Class MsgTbl;

   SCHEDULER_Class Scheduler;
   
   CFE_SB_PipeId_t CmdPipe;

} KIT_SCH_Class;

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** CMDMGR Data
   */
   uint16   ValidCmdCnt;
   uint16   InvalidCmdCnt;

   /*
   ** TBLMGR Data
   */

   uint8    MsgTblLastLoadStatus;
   uint8    SchTblLastLoadStatus;

   uint16   MsgTblAttrErrCnt;
   uint16   SchTblAttrErrCnt;

   /*
   ** SCHTBL Data
   ** - At a minimum every sch-tbl variable effected by a reset must be included
   ** - These have been rearranged to align data words
   */

   uint32  SlotsProcessedCount;
   uint32  ScheduleActivitySuccessCount;
   uint32  ScheduleActivityFailureCount;
   uint32  ValidMajorFrameCount;
   uint32  MissedMajorFrameCount;
   uint32  UnexpectedMajorFrameCount;
   uint32  TablePassCount;
   uint32  ConsecutiveNoisyFrameCounter;
   uint16  SkippedSlotsCount;
   uint16  MultipleSlotsCount;
   uint16  SameSlotCount;
   uint16  SyncAttemptsLeft;
   uint16  LastSyncMETSlot;
   boolean IgnoreMajorFrame;
   boolean UnexpectedMajorFrame;

} OS_PACK KIT_SCH_HkPkt;

#define KIT_SCH_HK_TLM_LEN sizeof (KIT_SCH_HkPkt)

typedef struct {

   uint8    Header[CFE_SB_TLM_HDR_SIZE];

   /*
   ** SCHTBL Data
   */
   uint32  LastProcessCount;
   uint32  TimerId;
   uint32  TimeSemaphore;
   uint32  ClockAccuracy;
   uint32  WorstCaseSlotsPerMinorFrame;
   boolean IgnoreMajorFrame;
   uint8   SyncToMET;
   uint8   MajorFrameSource;

} OS_PACK KIT_SCH_DiagPkt;

#define KIT_SCH_DIAG_TLM_LEN sizeof (KIT_SCH_DiagPkt)


/*
** Exported Data
*/

extern KIT_SCH_Class  KitSch;


/*
** Exported Functions
*/

/******************************************************************************
** Function: KIT_SCH_Main
**
*/
void KIT_SCH_Main(void);


/******************************************************************************
** Function: KIT_SCH_NoOpCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean KIT_SCH_NoOpCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


/******************************************************************************
** Function: KIT_SCH_ResetAppCmd
**
** Notes:
**   1. Function signature must match the CMDMGR_CmdFuncPtr definition
**
*/
boolean KIT_SCH_ResetAppCmd(void* ObjDataPtr, const CFE_SB_MsgPtr_t MsgPtr);


#endif /* _kit_sch_app_ */

