/*
**
** File:  ut_cfe_evs_stubs.h
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
** $Id: ut_cfe_evs_stubs.h 1.2 2015/03/06 14:34:30EST sstrege Exp  $
**
** Purpose: cFE Event Services Header file for unit test stubs
**
** $Log: ut_cfe_evs_stubs.h  $
** Revision 1.2 2015/03/06 14:34:30EST sstrege 
** Added copyright information
** Revision 1.1 2011/05/04 11:20:19EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/inc/project.pj
** Revision 1.1 2011/04/08 16:25:53EDT rmcgraw 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/CFS-REPOSITORY/cf/fsw/unit_test/ut-assert/inc/project.pj
** Revision 1.1 2011/02/15 11:12:33EST sslegel 
** Initial revision
** Member added to project c:/MKSDATA/MKS-REPOSITORY/FSW-TOOLS-REPOSITORY/ut-assert/inc/project.pj
**
*/

#ifndef UT_CFE_EVS_STUBS_H_
#define UT_CFE_EVS_STUBS_H_

typedef enum 
{
    UT_CFE_EVS_REGISTER_INDEX,
    UT_CFE_EVS_SENDEVENT_INDEX,
    UT_CFE_EVS_SENDTIMEDEVENT_INDEX,
    UT_CFE_EVS_SENDEVENTWITHAPPID_INDEX,
    UT_CFE_EVS_MAX_INDEX
} Ut_CFE_EVS_INDEX_t;

typedef struct
{
    int32 (*CFE_EVS_Register)(void *Filters, uint16 NumEventFilters, uint16 FilterScheme);
    int32 (*CFE_EVS_SendEvent)(uint16 EventID, uint16 EventType, char *EventText);
} Ut_CFE_EVS_HookTable_t;

typedef struct
{
    int32   Value;
    uint32  Count;
} Ut_CFE_EVS_ReturnCodeTable_t;

void Ut_CFE_EVS_Reset(void);
void Ut_CFE_EVS_SetFunctionHook(uint32 Index, void *FunPtr);
void Ut_CFE_EVS_SetReturnCode(uint32 Index, int32 RtnVal, uint32 CallCnt);

#endif 
