
/*
 * Filename: uttest.c
 *
 *  Copyright � 2007-2014 United States Government as represented by the 
 *  Administrator of the National Aeronautics and Space Administration. 
 *  All Other Rights Reserved.  
 *
 *  This software was created at NASA's Goddard Space Flight Center.
 *  This software is governed by the NASA Open Source Agreement and may be 
 *  used, distributed and modified only pursuant to the terms of that 
 *  agreement. 
 *
 * Purpose: This file contains functions to implement a standard way to execute unit tests.
 *
 */

/*
 * Includes
 */

#include "common_types.h"
#include "utassert.h"
#include "utlist.h"

/*
 * Type Definitions
 */

typedef struct {
    void    (*Test)(void);
    void    (*Setup)(void);
    void    (*Teardown)(void);
    char     *TestName;
} UtTestDataBaseEntry_t;

/*
 * Local Data
 */

UtListHead_t    UtTestDataBase;
uint32          UtTestsExecutedCount = 0;

/*
 * Function Definitions
 */

void UtTest_Add(void (*Test)(void), void (*Setup)(void), void (*Teardown)(void), char *TestName)
{
    UtTestDataBaseEntry_t   UtTestDataBaseEntry;

    UtTestDataBaseEntry.Test = Test;
    UtTestDataBaseEntry.Setup = Setup;
    UtTestDataBaseEntry.Teardown = Teardown;
    UtTestDataBaseEntry.TestName = TestName;
    UtList_Add(&UtTestDataBase, &UtTestDataBaseEntry, sizeof(UtTestDataBaseEntry_t), 0);
}

int UtTest_Run(void)
{
    uint32                   i;
    UtListNode_t            *UtListNode;
    UtTestDataBaseEntry_t   *UtTestDataBaseEntry;
    
    if (UtTestDataBase.NumberOfEntries > 0) {
        
        UtListNode = UtTestDataBase.First;
        for (i=0; i < UtTestDataBase.NumberOfEntries; i++) {
            
            UtTestDataBaseEntry = UtListNode->Data;

            #ifdef UT_VERBOSE
            if (strlen(UtTestDataBaseEntry->TestName) > 0) { printf("\nRunning Test: %s\n", UtTestDataBaseEntry->TestName); }
            #endif

            if (UtTestDataBaseEntry->Setup)    { UtTestDataBaseEntry->Setup(); }
            if (UtTestDataBaseEntry->Test)     { UtTestDataBaseEntry->Test(); UtTestsExecutedCount++; }
            if (UtTestDataBaseEntry->Teardown) { UtTestDataBaseEntry->Teardown(); }

            UtListNode = UtListNode->Next;
        }
    }

    printf("\n");
    printf("Tests Executed:    %lu\n", UtTestsExecutedCount);
    printf("Assert Pass Count: %lu\n", UtAssert_GetPassCount());
    printf("Assert Fail Count: %lu\n", UtAssert_GetFailCount());
    
    UtList_Reset(&UtTestDataBase);

    return (UtAssert_GetFailCount() > 0);
}
