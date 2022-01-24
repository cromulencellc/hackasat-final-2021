/*
**
** File:  ut_cfe_tbl_stubs.h
**
**  Copyright � 2007-2014 United States Government as represented by the 
**  Administrator of the National Aeronautics and Space Administration. 
**  All Other Rights Reserved.  
**
**  This software was created at NASA's Goddard Space Flight Center.
**  This software is governed by the NASA Open Source Agreement and may be 
**  used, distributed and modified only pursuant to the terms of that 
**  agreement.
**
** $Id: ut_cfe_tbl_stubs.h 1.2 2015/03/06 14:34:34EST sstrege Exp  $
**
** Purpose: cFE Table Services Header file for unit test stubs
**
** $Log: ut_cfe_tbl_stubs.h  $
** Revision 1.2 2015/03/06 14:34:34EST sstrege 
** Added copyright information
** Revision 1.1 2011/05/04 11:20:23EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/inc/project.pj
** Revision 1.1 2011/04/08 16:25:56EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/inc/project.pj
** Revision 1.2 2011/02/18 15:57:43EST sslegel 
** Added new hooks and return codes
** Changed Ut_CFE_TBL_LoadHook to automatically call the table validate function
** Revision 1.1 2011/02/15 11:12:35EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_TBL_STUBS_H_
#define UT_CFE_TBL_STUBS_H_

typedef enum 
{
    UT_CFE_TBL_REGISTER_INDEX,
    UT_CFE_TBL_LOAD_INDEX,
    UT_CFE_TBL_MANAGE_INDEX,
    UT_CFE_TBL_GETADDRESS_INDEX,
    UT_CFE_TBL_GETADDRESSES_INDEX,
    UT_CFE_TBL_GETSTATUS_INDEX,
    UT_CFE_TBL_GETINFO_INDEX,
    UT_CFE_TBL_MAX_INDEX
} Ut_CFE_TBL_INDEX_t;

typedef struct
{
    int32 (*CFE_TBL_Register)(CFE_TBL_Handle_t*, const char *,uint32, uint16, CFE_TBL_CallbackFuncPtr_t);
    int32 (*CFE_TBL_Load)(CFE_TBL_Handle_t, CFE_TBL_SrcEnum_t, const void *);
    int32 (*CFE_TBL_Manage)(CFE_TBL_Handle_t);
    int32 (*CFE_TBL_GetAddress)(void **, CFE_TBL_Handle_t);
    int32 (*CFE_TBL_GetAddresses)(void **[], uint16, const CFE_TBL_Handle_t []);
    int32 (*CFE_TBL_GetInfo)(CFE_TBL_Info_t *TblInfoPtr, const char *TblName);
} Ut_CFE_TBL_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
} Ut_CFE_TBL_ReturnCodeTable_t;

void Ut_CFE_TBL_Reset(void);
void Ut_CFE_TBL_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_TBL_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);

#endif 
