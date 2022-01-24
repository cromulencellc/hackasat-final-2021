
#ifndef _eps_cfg_tbl_h_
#define _eps_cfg_tbl_h_

#include "app_cfg.h"


/***********************/
/** Macro Definitions **/
/***********************/

#define EPS_CFG_CREATE_FILE_ERR_EID               (EPS_CFG_BASE_EID + 0)
#define EPS_CFG_LOAD_TYPE_ERR_EID                 (EPS_CFG_BASE_EID + 1)
#define EPS_CFG_LOAD_EMPTY_ERR_EID                (EPS_CFG_BASE_EID + 2)
#define EPS_CFG_LOAD_UPDATE_ERR_EID               (EPS_CFG_BASE_EID + 3)
#define EPS_CFG_LOAD_OPEN_ERR_EID                 (EPS_CFG_BASE_EID + 4)
#define EPS_CFG_DEBUG_EID                         (EPS_CFG_BASE_EID + 5)
#define EPS_CFG_INIT_DEBUG_EID                    (EPS_CFG_BASE_EID + 6)
#define EPS_CFG_LOAD_TBL_INFO_EID                 (EPS_CFG_BASE_EID + 7)
#define EPS_CFG_LOAD_TBL_ERR_EID                  (EPS_CFG_BASE_EID + 8)
#define EPS_CFG_REMOVE_PKT_SUCCESS_EID            (EPS_CFG_BASE_EID + 9)

#define EPS_CFG_INIT_EVS_TYPE CFE_EVS_DEBUG

/**********************/
/** Type Definitions **/
/**********************/
#define EPS_CFG_ENTRY_DISABLED      255

#define EPS_SWITCH_MASK_MAX_VALUE   0b111111111 /* 9 Entries With Value 1 0b111111111 */
#define EPS_MODE_NAME_LEN           64

/*EPS_CFG_ENTRY_DISABLED
*/

#define  EPS_CFG_MODE_TBL      0
#define  EPS_CFG_MODE_OBJ      1
#define  EPS_CFG_OBJ_CNT       2
#define  EPS_CFG_MODE_TBL_OBJ_NAME   "mode-table"
#define  EPS_CFG_MODE_OBJ_NAME     "mode"



/******************************************************************************
** Table Struture
*/
typedef struct {
   
    uint8_t ModeIndex;
    uint8_t Enabled;
   //  uint8_t StartupMode;
    uint8_t ModeSwitchTbl[EPS_SWITCH_CNT];
    char    ModeName[EPS_MODE_NAME_LEN];

} EPS_CFG_ModeEntry;



typedef struct {
   uint8_t StartupMode;
   EPS_CFG_ModeEntry ModeTbl[EPS_MODE_CNT];
} EPS_CFG_Tbl;



/*
** Table Owner Callback Functions
*/

/* Return pointer to owner's table data */
typedef const EPS_CFG_Tbl* (*EPS_CFG_GetTblPtrFcn)(void);
            
/* Table Owner's function to load all table data */
typedef boolean (*EPS_CFG_LoadTblFcn)(EPS_CFG_Tbl* NewTbl); 

/* Table Owner's function to load a single table pkt. The JSON object/container is an array */
typedef boolean (*EPS_CFG_LoadTblEntryFcn)(uint16 PktIdx, EPS_CFG_ModeEntry* NewPkt);

/******************************************************************************
** Command Definitions
*/


/******************************************************************************
** Class Struture
*/

typedef struct {

   /*
   ** App Framework
   */
   uint8    LastLoadStatus;
   uint16   AttrErrCnt;
   uint16   EntryLoadCnt;
   uint16   CurMsgId; /* Most recent AppId processed by the callback function */
   uint16   ModeEntryCnt;

   EPS_CFG_GetTblPtrFcn    GetTblPtrFunc;
   EPS_CFG_LoadTblFcn      LoadTblFunc;
   EPS_CFG_LoadTblEntryFcn LoadTblEntryFunc; 
   
   boolean                  StartupModeSet;
   EPS_CFG_Tbl              Tbl;

   /*
   ** Tables
   */
   JSON_Class Json;
   JSON_Obj   JsonObj[EPS_CFG_OBJ_CNT];
   char       JsonFileBuf[JSON_MAX_FILE_CHAR];   
   jsmntok_t  JsonFileTokens[JSON_MAX_FILE_TOKENS];

} EPS_CFG_Class;

/************************/
/** Exported Functions **/
/************************/

/******************************************************************************
** Function: EPS_CFG_Constructor
**
** Initialize the example cFE Table object.
**
** Notes:
**   None
*/
void EPS_CFG_Constructor(EPS_CFG_Class*  EpsCfgPtr,
                       EPS_CFG_GetTblPtrFcn    GetTblPtrFunc,
                       EPS_CFG_LoadTblFcn      LoadTblFunc, 
                       EPS_CFG_LoadTblEntryFcn LoadTblEntryFunc);


/******************************************************************************
** Function: EPS_CFG_ResetStatus
**
** Reset counters and status flags to a known reset state.  The behavour of
** the table manager should not be impacted. The intent is to clear counters
** and flags to a known default state for telemetry.
**
*/
void EPS_CFG_ResetStatus(void);


/******************************************************************************
** Function: EPS_CFG_GetTblPtr
**
** Return a pointer to the sb-pkt table.
**
** Notes:
**   1. Function signature must match EPS_CFG_GetTblPtr
**
*/
const EPS_CFG_Tbl* EPS_CFG_GetTblPtr(void);

/******************************************************************************
** Function: EPS_CFG_LoadTbl
**
**
** Notes:
**   1. No validity checks are performed on the table data.
**   2. Function signature must match SBNTBL_LoadTbl
**
*/
boolean EPS_CFG_LoadTbl(EPS_CFG_Tbl* NewTbl);

/******************************************************************************
** Function: EPS_CFG_LoadTblEntry
**
** Load a single message table entry
**
** Notes:
**   1. Range checking is not performed on the parameters.
**   2. Function signature must match EPS_CFG_LoadTblEntry
**
*/
boolean EPS_CFG_LoadTblEntry(uint16 PktIdx, EPS_CFG_ModeEntry* PktArray);

/******************************************************************************
** Function: EPS_CFG_LoadCmd
**
** Command to load the table.
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean EPS_CFG_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename);


/******************************************************************************
** Function: EPS_CFG_DumpCmd
**
** Command to dump the table.
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager.
**
*/
boolean EPS_CFG_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename);


/******************************************************************************
** Function: EPS_CFG_SetTblToUnused
**
*/
void EPS_CFG_SetTblToUnused(EPS_CFG_Tbl* TblPtr);

/******************************************************************************
** Function: EPS_CFG_SetTblToUnused
**
*/
void EPS_CFG_SetEntryToUnused(EPS_CFG_ModeEntry* EntryPtr);


/******************************************************************************
** Function: EPS_ModeArray_To_Mask
**
*/
uint16_t EPS_ModeArray_To_Mask(const uint8_t *ModeTblPtr);

/******************************************************************************
** Function: EPS_ModeMask_To_ModeArray
**
*/
void EPS_ModeMask_To_ModeArray(uint8_t *ModeTblPtr, uint16_t ModeTblMask);

#endif