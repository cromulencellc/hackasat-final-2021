/*
** Purpose: Implement the MQTT-to-SB Table
**
** Notes:
**   1. Initial OSK MQTT App based on a January 2021 refactor of Alan Cudmore's
**      MQTT App https://github.com/alanc98/mqtt_app. 
**
** License:
**   Preserved original https://github.com/alanc98/mqtt_app Apache License 2.0.
**
** References:
**   1. OpenSatKit Object-based Application Developers Guide.
**   2. cFS Application Developer's Guide.
*/

/*
** Include Files:
*/

#include <string.h>

#include "app_cfg.h"
#include "eps_cfg_tbl.h"
#include "eps_logger.h"

/*
** Global File Data
*/
#define  JSON  &(EpsCfg->Json)  /* Convenience macro */
static EPS_CFG_Class*  EpsCfg = NULL;
static EPS_CFG_Tbl tempTbl = {0};

/*
** Local Function Prototypes
*/


/*
** Local File Function Prototypes
*/
static boolean WriteJsonEntry(int32 FileHandle, const EPS_CFG_ModeEntry* Entry, boolean FirstEntryWritten);
/******************************************************************************
** Function: JSON Callbacks
**
** Notes:
**   1. These functions must have the same function signature as 
**      JSON_ContainerFuncPtr.
*/
static boolean StartupEntryCallback (void* UserData, int TokenIdx);
static boolean ConfigEntryCallback (void* UserData, int TokenIdx);


/******************************************************************************
** Function: EPS_CFG_Constructor
**
*/
void EPS_CFG_Constructor(EPS_CFG_Class*  EpsCfgPtr,
                       EPS_CFG_GetTblPtrFcn    GetTblPtrFunc,
                       EPS_CFG_LoadTblFcn      LoadTblFunc, 
                       EPS_CFG_LoadTblEntryFcn LoadTblEntryFunc)
{
 
   EpsCfg = EpsCfgPtr;


   CFE_PSP_MemSet((void*)EpsCfg, 0, sizeof(EPS_CFG_Class));
   EPS_CFG_SetTblToUnused(&(EpsCfg->Tbl));

   EpsCfg->GetTblPtrFunc    = GetTblPtrFunc;
   EpsCfg->LoadTblFunc      = LoadTblFunc;
   EpsCfg->LoadTblEntryFunc = LoadTblEntryFunc; 

   JSON_Constructor(JSON, EpsCfg->JsonFileBuf, EpsCfg->JsonFileTokens);

   JSON_ObjConstructor(&(EpsCfg->JsonObj[EPS_CFG_MODE_TBL]),
                       EPS_CFG_MODE_TBL_OBJ_NAME,
                       StartupEntryCallback,
                       (void *)&(EpsCfg->Tbl));

   JSON_RegContainerCallback(JSON, &(EpsCfg->JsonObj[EPS_CFG_MODE_TBL]));
   
   JSON_ObjConstructor(&(EpsCfg->JsonObj[EPS_CFG_MODE_OBJ]),
                       EPS_CFG_MODE_OBJ_NAME,
                       ConfigEntryCallback,
                       (void *)&(EpsCfg->Tbl.ModeTbl));

   JSON_RegContainerCallback(JSON, &(EpsCfg->JsonObj[EPS_CFG_MODE_OBJ]));
   

} /* End EPS_CFG_Constructor() */


/******************************************************************************
** Function:  EPS_CFG_ResetStatus
**
*/
void EPS_CFG_ResetStatus()
{

   EpsCfg->LastLoadStatus  = TBLMGR_STATUS_UNDEF;
   EpsCfg->AttrErrCnt      = 0;
   EpsCfg->EntryLoadCnt      = 0;
   EpsCfg->ModeEntryCnt   = 0; 
   
   JSON_ObjArrayReset (EpsCfg->JsonObj, EPS_CFG_OBJ_CNT);

} /* End EPS_CFG_ResetStatus() */



/******************************************************************************
** Function: EPS_CFG_LoadCmd
**
** Notes:
**  1. Function signature must match TBLMGR_LoadTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
*/
boolean EPS_CFG_LoadCmd(TBLMGR_Tbl *Tbl, uint8 LoadType, const char* Filename)
{

   int EntryIdx;
   
   CFE_EVS_SendEvent(EPS_CFG_INIT_DEBUG_EID, EPS_CFG_INIT_EVS_TYPE, "EPS_CFG_LoadCmd() Entry. sizeof(EpsCfg->Tbl) = %d\n", sizeof(EpsCfg->Tbl));
   EPS_LOG_DEBUG("Loading EPS Configuration Table FIle %s, LoadType %d", Filename, LoadType);
   /* 
   ** Reset status, object modified flags, and data. A non-zero BufLim
   ** value is used to determine whether a packet was loaded.
   */
   EPS_CFG_ResetStatus();  
   
   CFE_PSP_MemSet(&tempTbl, 0, sizeof(tempTbl));
   EPS_CFG_SetTblToUnused(&tempTbl);

   if (JSON_OpenFile(JSON, Filename)) {
  
      CFE_EVS_SendEvent(EPS_CFG_INIT_DEBUG_EID, EPS_CFG_INIT_EVS_TYPE, "EPS_CFG_LoadCmd() - Successfully prepared file %s\n", Filename);
      EPS_LOG_DEBUG("EPS_CFG_Table File Read Into Memory");
      //DEBUG JSON_PrintTokens(&Json,JsonFileTokens[0].size);
      //DEBUG JSON_PrintTokens(&Json,50);
    
      JSON_ProcessTokens(JSON);

      /* 
      ** Only process command if no attribute errors. No need to send an event
      ** message if there are attribute errors since events are sent for each
      ** error.  Checking at least one packet is a simple check but that's all
      ** that can be done because the table doesn't have any other constraints.
      ** A table with no packets is considered erroneous.
      */
      if (EpsCfg->AttrErrCnt == 0) {
      
        if (EpsCfg->EntryLoadCnt > 0) {
         
            if (LoadType == TBLMGR_LOAD_TBL_REPLACE) { 
                EpsCfg->LastLoadStatus = ((EpsCfg->LoadTblFunc)(&tempTbl) == TRUE) ? TBLMGR_STATUS_VALID : TBLMGR_STATUS_INVALID;

            } /* End if replace entire table */
            else if (LoadType == TBLMGR_LOAD_TBL_UPDATE) {
               CFE_EVS_SendEvent(EPS_CFG_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"Load type %d UPDATE_TBL not currently supported", LoadType);
               EpsCfg->LastLoadStatus = TBLMGR_STATUS_INVALID;
               //  EpsCfg->LastLoadStatus = TBLMGR_STATUS_VALID;
               //  if (EpsCfg->JsonObj[EPS_CFG_MODE_OBJ].Modified) {
                            
               //      for (EntryIdx=0; (EntryIdx < EPS_MODE_CNT) && (EpsCfg->LastLoadStatus == TBLMGR_STATUS_VALID); EntryIdx++) {

               //          if (EpsCfg->Tbl.ModeTbl[EntryIdx].ModeIndex != EPS_CFG_ENTRY_DISABLED) {

               //              if (!(EpsCfg->LoadTblEntryFunc)(EntryIdx, (EPS_CFG_ModeEntry*)EpsCfg->JsonObj[EPS_CFG_MODE_OBJ].UserData))
               //                  EpsCfg->LastLoadStatus = TBLMGR_STATUS_INVALID;
               //          }     
               //      } /* End packet array loop */                
               //  } /* End if at least one object modified */
               //  else {
                    
               //      /* This is an un explainable error */
               //      CFE_EVS_SendEvent(EPS_CFG_LOAD_EMPTY_ERR_EID, CFE_EVS_ERROR, "Load packet table update rejected.");

               //  } /* End if no objects in file */
               
            } /* End if update records */
            else {
               
                CFE_EVS_SendEvent(EPS_CFG_LOAD_TYPE_ERR_EID,CFE_EVS_ERROR,"Load packet table rejected. Invalid table command load type %d", LoadType);
            
            } /* End if invalid command option */ 
            
         } /* End if at least one packet loaded */
         else {
            
            CFE_EVS_SendEvent(EPS_CFG_LOAD_UPDATE_ERR_EID, CFE_EVS_ERROR,
                "Load packet table command rejected. %s didn't contain any packet defintions", Filename);
         
         } /* End if too many packets in table file */

      } /* End if no attribute errors */
            
   } /* End if valid file */
   else {
      
      //printf("**ERROR** Processing Packet Table file %s. Status = %d JSMN Status = %d\n",TEST_FILE, Json.FileStatus, Json.JsmnStatus);
      CFE_EVS_SendEvent(EPS_CFG_LOAD_OPEN_ERR_EID,CFE_EVS_ERROR,"Load packet table open failure for file %s. File Status = %s JSMN Status = %s",
        Filename, JSON_GetFileStatusStr(EpsCfg->Json.FileStatus), JSON_GetJsmnErrStr(EpsCfg->Json.JsmnStatus));
   
   } /* End if file processing error */

   EPS_LOG_INFO("EPS Table Load Complete. Status: %d", EpsCfg->LastLoadStatus);
   return (EpsCfg->LastLoadStatus == TBLMGR_STATUS_VALID);

} /* End of EPS_CFG_LoadCmd() */


/******************************************************************************
** Function: EPS_CFG_DumpCmd
**
** Notes:
**  1. Function signature must match TBLMGR_DumpTblFuncPtr.
**  2. Can assume valid table file name because this is a callback from 
**     the app framework table manager that has verified the file.
**  3. DumpType is unused.
**  4. File is formatted so it can be used as a load file. It does not follow
**     the cFE table file format. 
**  5. Creates a new dump file, overwriting anything that may have existed
**     previously
*/

boolean EPS_CFG_DumpCmd(TBLMGR_Tbl *Tbl, uint8 DumpType, const char* Filename)
{

   boolean  RetStatus = FALSE;
   boolean  FirstEntryWritten = FALSE;
   int32    FileHandle;
   uint16   EntryIdx;
   char     DumpRecord[256];
   const EPS_CFG_Tbl *EpsCfgPtr;
   char SysTimeStr[128];
   
   FileHandle = OS_creat(Filename, OS_WRITE_ONLY);

   if (FileHandle >= OS_FS_SUCCESS) {

      EpsCfgPtr = (EpsCfg->GetTblPtrFunc)();

      sprintf(DumpRecord,"\n{\n\"name\": \"EPS Configuration Table\",\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      CFE_TIME_Print(SysTimeStr, CFE_TIME_GetTime());
      
      sprintf(DumpRecord,"\"description\": \"Configuration for EPS MGR dumped at %s\",\n",SysTimeStr);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));


      /* 
      ** Mode Table
      **
      ** - Not all fields in ground table are saved in FSW so they are not
      **   populated in the dump file. However, the dump file can still
      **   be loaded.
      */
      
      sprintf(DumpRecord,"\"mode-table\": {\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"      \"startup-mode\": %d,\n", EpsCfgPtr->StartupMode);
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      sprintf(DumpRecord,"      \"mode-array\": [\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
      
      for (EntryIdx=0; EntryIdx < EPS_MODE_CNT; EntryIdx++) {
               
         if (WriteJsonEntry(FileHandle, &(EpsCfgPtr->ModeTbl[EntryIdx]), FirstEntryWritten)) FirstEntryWritten = TRUE;
              
      } /* End packet loop */

      sprintf(DumpRecord,"\n      ]\n   }\n}\n");
      OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

      RetStatus = TRUE;

      OS_close(FileHandle);

   } /* End if file create */
   else {
   
      CFE_EVS_SendEvent(EPS_CFG_CREATE_FILE_ERR_EID, CFE_EVS_ERROR,
                        "Error creating dump file '%s', Status=0x%08X", Filename, FileHandle);
   
   } /* End if file create error */

   return RetStatus;
   
} /* End of EPS_CFG_DumpCmd() */



/******************************************************************************
** Function: WriteJsonEntry
**
** Notes:
**   1. Can't end last record with a comma so logic checks that commas only
**      start to be written after the first packet has been written
*/
static boolean WriteJsonEntry(int32 FileHandle, const EPS_CFG_ModeEntry* Entry, boolean FirstEntryWritten) {
   
   boolean EntryWritten = FALSE;
   char DumpRecord[256];

    if (Entry->ModeIndex != EPS_CFG_ENTRY_DISABLED) {
      
        if (FirstEntryWritten) {
            sprintf(DumpRecord,",\n");
            OS_write(FileHandle,DumpRecord,strlen(DumpRecord));
        }
        
        sprintf(DumpRecord,"      \"mode\": {\n");
        OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

        uint16_t modeMask = EPS_ModeArray_To_Mask(Entry->ModeSwitchTbl);

        sprintf(DumpRecord,"         \"mode-index\": %d,\n         \"enabled\": %d,,\n         \"name\": \"%s\",\n         \"mode-switch-mask\": %d\n      }",
                Entry->ModeIndex, Entry->Enabled, Entry->ModeName, modeMask);

        OS_write(FileHandle,DumpRecord,strlen(DumpRecord));

        EntryWritten = TRUE;
        
   } /* End if MsgId record has been loaded */
   
   return EntryWritten;
} /* End WriteJsonEntry() */


/******************************************************************************
** Function: StartupEntryCallback
**
** Process a packet table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. ObjLoadCnt incremented for every packet, valid or invalid.
**      EntryLoadIdx index to stored new pkt and incremented for valid packets
*/
static boolean StartupEntryCallback (void* UserData, int TokenIdx)
{

   int  AttributeCnt = 0;
   int  JsonIntData;
   uint8_t StartupIdx;

   EpsCfg->JsonObj[EPS_CFG_MODE_TBL].Modified = FALSE; 
   

   if (JSON_GetValShortInt(JSON, TokenIdx, "startup-mode",      &JsonIntData)) { AttributeCnt++; StartupIdx = (uint8_t) JsonIntData; }
   // if (JSON_GetValShortInt(JSON, TokenIdx, "execution-delay",      &JsonIntData)) { AttributeCnt++; StartupIdx = (uint8_t) JsonIntData; }


   if (AttributeCnt == 1) {
   
        ++EpsCfg->EntryLoadCnt;
        if(StartupIdx < (EPS_MODE_CNT - 1)) {
            tempTbl.StartupMode = StartupIdx; 
        }
        
        EpsCfg->JsonObj[EPS_CFG_MODE_TBL].Modified = TRUE;
      

        CFE_EVS_SendEvent(EPS_CFG_INIT_DEBUG_EID, EPS_CFG_INIT_EVS_TYPE, 
                        "EPS_CFG.StartupEntryCallback() Startup Mode Index = %d", StartupIdx);
   } /* End if valid AttributeCnt */
   else {
	   
      ++EpsCfg->AttrErrCnt;     
      CFE_EVS_SendEvent(EPS_CFG_LOAD_TBL_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 1.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */

    CFE_EVS_SendEvent(EPS_CFG_INIT_DEBUG_EID, EPS_CFG_INIT_EVS_TYPE,
                    "EPS_CFG.StartupEntryCallback: EntryLoadCnt %d, AttrErrCnt %d, TokenIdx %d",
                    EpsCfg->EntryLoadCnt, EpsCfg->AttrErrCnt, TokenIdx);
      
   return EpsCfg->JsonObj[EPS_CFG_MODE_TBL].Modified;

} /* EntryCallback() */

/******************************************************************************
** Function: EntryCallback
**
** Process a packet table entry.
**
** Notes:
**   1. This must have the same function signature as JSON_ContainerFuncPtr.
**   2. ObjLoadCnt incremented for every packet, valid or invalid.
**      EntryLoadIdx index to stored new pkt and incremented for valid packets
*/
static boolean ConfigEntryCallback (void* UserData, int TokenIdx)
{

   int  AttributeCnt = 0;
   int  JsonIntData;
   char JsonStrData[EPS_MODE_NAME_LEN];
   EPS_CFG_ModeEntry Entry;
   
   EpsCfg->JsonObj[EPS_CFG_MODE_OBJ].Modified = FALSE; 
   

      
   EPS_CFG_SetEntryToUnused(&Entry); /* Default filter to always filtered in case not loaded by FilterCallback() */
   
   if (JSON_GetValShortInt(JSON, TokenIdx, "mode-index",        &JsonIntData)) { AttributeCnt++; Entry.ModeIndex = (uint8_t) JsonIntData; }
   if (JSON_GetValShortInt(JSON, TokenIdx, "enabled",           &JsonIntData)) { AttributeCnt++; Entry.Enabled = (uint8_t) JsonIntData; }
//    if (JSON_GetValShortInt(JSON, TokenIdx, "startup-mode",      &JsonIntData)) { AttributeCnt++; Entry.StartupMode = (uint8_t) JsonIntData; }
   if (JSON_GetValStr(JSON, TokenIdx, "name",              JsonStrData)) { AttributeCnt++; CFE_PSP_MemCpy(Entry.ModeName, JsonStrData, EPS_MODE_NAME_LEN); }
   if (JSON_GetValShortInt(JSON, TokenIdx, "mode-mask",  &JsonIntData)) { 
       AttributeCnt++;
       EPS_ModeMask_To_ModeArray(Entry.ModeSwitchTbl, (uint16_t)JsonIntData);
    }
   EPS_LOG_DEBUG("Mode Array Entry %d Processed. Found %d entries", EpsCfg->ModeEntryCnt, AttributeCnt);
   if (AttributeCnt == 4) {
   
        ++EpsCfg->EntryLoadCnt;
        CFE_PSP_MemCpy(&tempTbl.ModeTbl[EpsCfg->ModeEntryCnt], &Entry, sizeof(EPS_CFG_ModeEntry));   

        ++EpsCfg->ModeEntryCnt;
        
        EpsCfg->JsonObj[EPS_CFG_MODE_OBJ].Modified = TRUE;
      

        CFE_EVS_SendEvent(EPS_CFG_INIT_DEBUG_EID, EPS_CFG_INIT_EVS_TYPE, 
                        "EPS_CFG.EntryCallback() (Mode Index, Enabled, ModeName, ModeMask): %d, %d, %s, %d",
                        Entry.ModeIndex, Entry.Enabled, Entry.ModeName, JsonIntData);
   } /* End if valid AttributeCnt */
   else {
	   
      ++EpsCfg->AttrErrCnt;     
      CFE_EVS_SendEvent(EPS_CFG_LOAD_TBL_ERR_EID, CFE_EVS_ERROR, "Invalid number of attributes %d. Should be 6.",
                        AttributeCnt);
   
   } /* End if invalid AttributeCnt */

    CFE_EVS_SendEvent(EPS_CFG_INIT_DEBUG_EID, EPS_CFG_INIT_EVS_TYPE,
                    "EPS_CFG.EntryCallback: EntryLoadCnt %d, ModeEntryCnt %d, AttrErrCnt %d, TokenIdx %d",
                    EpsCfg->EntryLoadCnt, EpsCfg->ModeEntryCnt, EpsCfg->AttrErrCnt, TokenIdx);
      
   return EpsCfg->JsonObj[EPS_CFG_MODE_OBJ].Modified;

} /* EntryCallback() */


boolean EPS_CFG_ModeEntryValidate(EPS_CFG_ModeEntry* Entry) {\
   boolean valid = TRUE;
   if (Entry->ModeIndex > EPS_MODE_CNT-1) valid &= FALSE;
   if (Entry->Enabled > 1 ) valid &= FALSE;
//    if (Entry->StartupMode > 1) valid &= FALSE;
   for (int i=0; i<EPS_SWITCH_CNT; i++) {
       if (Entry->ModeSwitchTbl[i] > 1) {
           valid &= FALSE;
       }
   }
   uint16_t modeMask = EPS_ModeArray_To_Mask(Entry->ModeSwitchTbl);
   if (modeMask > EPS_SWITCH_MASK_MAX_VALUE) valid &= FALSE;
   return valid;
}

/******************************************************************************
** Function: EPS_ModeArray_To_Mask
**
*/
uint16_t EPS_ModeArray_To_Mask(const uint8_t *ModeTblPtr) {
    uint16_t modeMask = 0;
    for (int i=0; i<EPS_SWITCH_CNT; i++) {
        modeMask |= (uint16_t) ((ModeTblPtr[i] & 0x1) << i);
    }
    return modeMask;
}


/******************************************************************************
** Function: EPS_ModeMask_To_ModeArray
**
*/
void EPS_ModeMask_To_ModeArray(uint8_t *ModeTblPtr, uint16_t ModeTblMask) {
   //  uint16_t ModeTblSwapped = __builtin_bswap16(ModeTblMask);
    uint8 value;
    for (int i=0; i<EPS_SWITCH_CNT; i++) {
        value =  (uint8_t) ((ModeTblMask >> i) & 0x1);
        ModeTblPtr[i] = value;
    }
}

/******************************************************************************
** Function: EPS_CFG_GetTblPtr
**
*/
const EPS_CFG_Tbl* EPS_CFG_GetTblPtr()
{

   return &(EpsCfg->Tbl);

} /* End EPS_CFG_GetTblPtr() */

/******************************************************************************
** Function: EPS_CFG_LoadTbl
**
*/
boolean EPS_CFG_LoadTbl(EPS_CFG_Tbl* NewTbl)
{

   uint16   EntryIdx;
   uint16   EntryCnt = 0;
   uint16   InvalidCnt = 0;
   boolean  RetStatus = TRUE;

   for (EntryIdx=0; EntryIdx < EPS_MODE_CNT; EntryIdx++) {
      if (NewTbl->ModeTbl[EntryIdx].ModeIndex != EPS_CFG_ENTRY_DISABLED) {
         if(EPS_CFG_ModeEntryValidate(&(NewTbl->ModeTbl[EntryIdx]))) {
            ++EntryCnt;
         }  else {
            ++InvalidCnt;
         }
      }
   } /* End pkt loop */

   if (InvalidCnt == 0 && EntryCnt > 0) {
      EpsCfg->LastLoadStatus = TRUE;
      CFE_PSP_MemCpy(&(EpsCfg->Tbl), NewTbl, sizeof(EPS_CFG_Tbl));
      CFE_EVS_SendEvent(EPS_CFG_LOAD_TBL_INFO_EID, CFE_EVS_INFORMATION,
                        "Successfully loaded new table with %d entries", EntryCnt);
      EPS_LOG_DEBUG("Successfully loaded new table with %d entries", EntryCnt);
   }
   else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(EPS_CFG_LOAD_TBL_ERR_EID, CFE_EVS_INFORMATION,
                        "Attempted to load new table with %d entries. %d entries invalid",
                        EntryCnt, InvalidCnt);
      EPS_LOG_ERROR("Attempted to load new table with %d entries. %d entries invalid",
                        EntryCnt, InvalidCnt);
   }

   return RetStatus;

} /* End EPS_CFG_LoadTbl() */

/******************************************************************************
** Function: EPS_CFG_LoadTblEntry
**
*/
boolean EPS_CFG_LoadTblEntry(uint16 EntryIdx, EPS_CFG_ModeEntry* EntryArray)
{
   boolean  RetStatus = TRUE;
   EPS_CFG_ModeEntry* NewEntry = &(EpsCfg->Tbl.ModeTbl[EntryIdx]); 

   if(EPS_CFG_ModeEntryValidate(&(EntryArray[EntryIdx])))
   {
      CFE_PSP_MemCpy(NewEntry,&EntryArray[EntryIdx],sizeof(EPS_CFG_ModeEntry));
      CFE_EVS_SendEvent(EPS_CFG_LOAD_TBL_INFO_EID, CFE_EVS_INFORMATION,
                        "Loaded table entry %d with Name %s, Enabled %d, Mode Switch Mask 0x%02X",
                        EntryIdx, NewEntry->ModeName, (unsigned int) EPS_ModeArray_To_Mask(NewEntry->ModeSwitchTbl));
   } else {
      
      RetStatus = FALSE;
      CFE_EVS_SendEvent(EPS_CFG_LOAD_TBL_ERR_EID,CFE_EVS_ERROR,
                        "Error Loading table entry %d",
                        EntryIdx);
   }

   return RetStatus;

} /* End EPS_CFG_LoadTblEntry() */

/******************************************************************************
** Function: EPS_CFG_SetEntryToUnused
**
**
*/
void EPS_CFG_SetEntryToUnused(EPS_CFG_ModeEntry* EntryPtr)
{

   CFE_PSP_MemSet(EntryPtr, 0, sizeof(EPS_CFG_ModeEntry));
   EntryPtr->ModeIndex = EPS_CFG_ENTRY_DISABLED;
   EntryPtr->Enabled = 0;
//    EntryPtr->StartupMode = 0;
} /* End EPS_CFG_SetEntryToUnused() */

/******************************************************************************
** Function: EPS_CFG_SetTblToUnused
**
**
*/
void EPS_CFG_SetTblToUnused(EPS_CFG_Tbl* TblPtr)
{
  
   uint16 EntryIdx;
   
   CFE_PSP_MemSet(TblPtr, 0, sizeof(EPS_CFG_Tbl));

   for (EntryIdx=0; EntryIdx < EPS_MODE_CNT; EntryIdx++) {
       EPS_CFG_SetEntryToUnused(&TblPtr->ModeTbl[EntryIdx]);
   }
   
} /* End EPS_CFG_SetTblToUnused() */


