/*================================================================================*
** File:  ut_os_stubs.c
** Owner: Tam Ngo
** Date:  May 2013
**================================================================================*/

#include "ut_os_stubs.h"

/*--------------------------------------------------------------------------------*
** Macros
**--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*
** Data types
**--------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------*
** External global variables
**--------------------------------------------------------------------------------*/

/* As defined in osfilesys.c */
extern OS_VolumeInfo_t  OS_VolumeTable[NUM_TABLE_ENTRIES];

/*--------------------------------------------------------------------------------*
** Global variables
**--------------------------------------------------------------------------------*/

UT_OsLogInfo_t  g_logInfo;

/*--------------------------------------------------------------------------------*
** Sets up test environment
**--------------------------------------------------------------------------------*/
void UT_os_setup(const char* logFileName)
{
    g_logInfo.nPassed = 0;
    g_logInfo.nFailed = 0;
    g_logInfo.nMir    = 0;
    g_logInfo.nNa     = 0;
    g_logInfo.nUof    = 0;
    g_logInfo.nTsf    = 0;

    g_logInfo.totalTstCnt = 0;

    g_logInfo.apiCnt = 0;
    memset(g_logInfo.apis, 0x00, sizeof(g_logInfo.apis));

    g_logInfo.verboseLevel = 0;

#ifndef OS_USE_EMBEDDED_PRINTF
    g_logInfo.logFD = NULL;
    if (logFileName != NULL)
    {
        g_logInfo.logFD = fopen(logFileName, "w");
        if (g_logInfo.logFD == NULL)
            fprintf(stderr, "Error: Can't open file %s\n", logFileName);
    }
#endif  /* !OS_USE_EMBEDDED_PRINTF */
}

/*--------------------------------------------------------------------------------*
** Tears down test setup
**--------------------------------------------------------------------------------*/
void UT_os_teardown(const char* testSuiteName)
{
    UT_os_log_test_summaries(testSuiteName);
    UT_os_log_test_results(testSuiteName);

#ifndef OS_USE_EMBEDDED_PRINTF
    if (g_logInfo.logFD != NULL)
    {
        fflush(g_logInfo.logFD);
        fclose(g_logInfo.logFD);
        g_logInfo.logFD = NULL;
    }
#endif  /* !OS_USE_EMBEDDED_PRINTF */

    OS_printf("\n%s HAS %d tests total.\n", testSuiteName, (int)g_logInfo.totalTstCnt);
    OS_printf("%s PASSED %d tests.\n", testSuiteName, (int)g_logInfo.nPassed);
    OS_printf("%s FAILED %d tests.\n", testSuiteName, (int)g_logInfo.nFailed);

    if (g_logInfo.nMir > 0)
    {
        OS_printf("%s HAS %d tests requiring manual inspection.\n",
                  testSuiteName, (int)g_logInfo.nMir);
    }

    if (g_logInfo.nUof > 0)
    {
        OS_printf("%s SKIPPED %d tests relating to OS-call-failure.\n",
                  testSuiteName, (int)g_logInfo.nUof);
    }

    if (g_logInfo.nNa > 0)
    {
        OS_printf("%s SKIPPED %d tests not applicable to the platform.\n",
                  testSuiteName, (int)g_logInfo.nNa);
    }

    if (g_logInfo.nTsf > 0)
    {
        OS_printf("%s SKIPPED %d tests failing test setup.\n",
                  testSuiteName, (int)g_logInfo.nTsf);
    }

    OS_printf("\n");
}

/*--------------------------------------------------------------------------------*
** Logs an API test descriptions
**--------------------------------------------------------------------------------*/
void UT_os_log_api(UT_OsApiInfo_t* apiPtr)
{
    if (apiPtr != NULL)
    {
        int32 i = 0;
        UT_OsApiInfo_t*  apiInfo  = NULL;
        UT_OsTestDesc_t* testInfo = NULL;
        UT_OsTestDesc_t* testPtr  = NULL;

        apiInfo = &(g_logInfo.apis[g_logInfo.apiCnt]);
        memset(apiInfo, 0x00, sizeof(UT_OsApiInfo_t));

        strncpy(apiInfo->name, apiPtr->name, strlen(apiPtr->name));

        apiInfo->tstCnt = apiPtr->tstCnt;
        g_logInfo.totalTstCnt += apiInfo->tstCnt;

        for (i=0; i < apiInfo->tstCnt; i++)
        {
            testPtr  = &(apiPtr->tests[i]);
            testInfo = &(apiInfo->tests[i]);

            memset(testInfo, 0x00, sizeof(UT_OsTestDesc_t));

            strncpy(testInfo->name,   testPtr->name,   strlen(testPtr->name));
            strncpy(testInfo->result, testPtr->result, strlen(testPtr->result));

            if (strcmp(testInfo->result, UT_OS_PASSED) == 0)
                g_logInfo.nPassed++;
            else if (strcmp(testInfo->result, UT_OS_FAILED) == 0)
                g_logInfo.nFailed++;
            else if (strcmp(testInfo->result, UT_OS_MIR) == 0)
                g_logInfo.nMir++;
            else if (strcmp(testInfo->result, UT_OS_NA) == 0)
                g_logInfo.nNa++;
            else if (strcmp(testInfo->result, UT_OS_UOF) == 0)
                g_logInfo.nUof++;
            else if (strcmp(testInfo->result, UT_OS_TSF) == 0)
            	g_logInfo.nTsf++;
        }

        g_logInfo.apiCnt++;
    }
}

/*--------------------------------------------------------------------------------*
** Set verbose level for logging
**--------------------------------------------------------------------------------*/
void UT_os_set_log_verbose(uint32 vLevel)
{
    switch (vLevel)
    {
        case UT_OS_LOG_OFF:
        case UT_OS_LOG_MINIMAL:
        case UT_OS_LOG_MODERATE:
        case UT_OS_LOG_EVERYTHING:
            g_logInfo.verboseLevel = vLevel;
            break;

        default:
            g_logInfo.verboseLevel = UT_OS_LOG_EVERYTHING;
            break;
    }
}

/*--------------------------------------------------------------------------------*
** Set return code
**--------------------------------------------------------------------------------*/
void UT_os_set_return_code(UT_OsReturnCode_t* ret, int32 count, int32 value)
{
    if (ret != NULL)
    {
        ret->count = count;
        ret->value = value;
    }
}

/*--------------------------------------------------------------------------------*
** Log test summaries
**--------------------------------------------------------------------------------*/
void UT_os_log_test_summaries(const char* testSuiteName)
{
    const char* tstName = testSuiteName;

    if (tstName == NULL)
        tstName = " ";

    UT_OS_LOG_MACRO("\n========================================================\n");
    UT_OS_LOG_MACRO("%s TOTAL APIs: %d\n", tstName, (int)g_logInfo.apiCnt);
    UT_OS_LOG_MACRO("%s TOTAL TEST CASES: %d\n\n", tstName, (int)g_logInfo.totalTstCnt);
    UT_OS_LOG_MACRO("%s PASSED %3d tests.\n", tstName, (int)g_logInfo.nPassed);
    UT_OS_LOG_MACRO("%s FAILED %3d tests.\n", tstName, (int)g_logInfo.nFailed);

    UT_OS_LOG_MACRO("\n%s contains %2d tests that are untested OS-call-failure.\n",
                          tstName, (int)g_logInfo.nUof);
    UT_OS_LOG_MACRO("%s contains %2d tests that are manual-inspection-required.\n",
                          tstName, (int)g_logInfo.nMir);
    UT_OS_LOG_MACRO("\n%s contains %2d tests that are not-applicable.\n",
                          tstName, (int)g_logInfo.nNa);

    if (g_logInfo.nTsf > 0)
    {
        UT_OS_LOG_MACRO("\n%s contains %2d tests that are test-setup-failure.\n",
                              tstName, (int)g_logInfo.nTsf);
    }

    UT_OS_LOG_MACRO("========================================================\n")
}

/*--------------------------------------------------------------------------------*
** Print log info
**--------------------------------------------------------------------------------*/
void UT_os_log_test_results(const char* testSuiteName)
{
    int32 i = 0, j = 0;
    UT_OsApiInfo_t* apiPtr = NULL;
    const char* tstName = testSuiteName;

    if (tstName == NULL)
        tstName = " ";

    if (g_logInfo.verboseLevel >= UT_OS_LOG_MODERATE)
    {
        UT_OS_LOG_MACRO("\n--------------------------------------------------------\n");
        UT_OS_LOG_MACRO("%s TOTAL APIs: %d\n", tstName, (int)g_logInfo.apiCnt);
        UT_OS_LOG_MACRO("%s TOTAL TEST CASES: %d\n", tstName, (int)g_logInfo.totalTstCnt);
        UT_OS_LOG_MACRO("--------------------------------------------------------");

        for (i=0; i < g_logInfo.apiCnt; i++)
        {
            apiPtr = &(g_logInfo.apis[i]);
            UT_OS_LOG_MACRO("\n    %s: %d\n", apiPtr->name, (int)apiPtr->tstCnt);
            for (j=0; j < apiPtr->tstCnt; j++)
            {
                UT_OS_LOG_MACRO("        %s [%s]\n", apiPtr->tests[j].name,
                                      apiPtr->tests[j].result);
            }
        }
    }

    UT_os_log_test_result_category("PASSED", UT_OS_PASSED, g_logInfo.nPassed);
    UT_os_log_test_result_category("FAILED", UT_OS_FAILED, g_logInfo.nFailed);
    UT_os_log_test_result_category("MANUAL-INSPECTION-REQUIRED", UT_OS_MIR, g_logInfo.nMir);
    UT_os_log_test_result_category("NOT-APPLICABLE", UT_OS_NA, g_logInfo.nNa);
    UT_os_log_test_result_category("UNTESTED-OS-CALL-FAILURE", UT_OS_UOF, g_logInfo.nUof);

    if (g_logInfo.nTsf > 0)
    {
        UT_os_log_test_result_category("TEST-SETUP-FAILURE", UT_OS_TSF, g_logInfo.nTsf);
    }

    if (g_logInfo.verboseLevel != UT_OS_LOG_OFF)
    {
        UT_OS_LOG_MACRO("\n\n");
    }
}

/*--------------------------------------------------------------------------------*
** Print a test-result category
**--------------------------------------------------------------------------------*/
void UT_os_log_test_result_category(const char* catName, const char* catKey, uint32 nCases)
{
    int32 i = 0, j = 0;
    UT_OsApiInfo_t* apiPtr = NULL;
    const char* inCatKey = catKey;
    char txtStr[UT_OS_LG_TEXT_LEN];
    const char* inCatName = catName;

    if (inCatKey == NULL)
        inCatKey = " ";

    if (inCatName == NULL)
        inCatName = " ";

    if (g_logInfo.verboseLevel >= UT_OS_LOG_MODERATE)
    {
        UT_OS_LOG_MACRO("\n--------------------------------------------------------\n");
        UT_OS_LOG_MACRO("  TOTAL TEST CASES %s -> %d\n", inCatName, (int)nCases);
        UT_OS_LOG_MACRO("--------------------------------------------------------\n");

        for (i=0; i < g_logInfo.apiCnt; i++)
        {
            apiPtr = &(g_logInfo.apis[i]);
            for (j=0; j < apiPtr->tstCnt; j++)
            {
                if (strcmp(apiPtr->tests[j].result, inCatKey) == 0)
                {
                    memset(txtStr, '\0', sizeof(txtStr));
                    UT_OS_LOG_MACRO("    %s [ ] %s - %s \n",
                                          inCatKey, apiPtr->name, apiPtr->tests[j].name);
                }
            }
        }
    }
}

/*--------------------------------------------------------------------------------*
** Print OS_VolumeTable[]
**--------------------------------------------------------------------------------*/
void UT_os_print_volumetable(const char* outputDesc)
{
#ifdef UT_VERBOSE
    int j;

    if (outputDesc)
        UT_OS_LOG_MACRO("%s",outputDesc)

    UT_OS_LOG_MACRO("\n--------------------------------------------------------------")
    UT_OS_LOG_MACRO("-------------------------------------------------------------\n")
    UT_OS_LOG_MACRO(" %-8s%-16s%-32s%-14s%-18s%-8s%-8s%-10s", "DEVNAME", "PHYSDEV", "VOLNAME", "MOUNTPNT", "VOLATILE?","FREE?","MOUNTED?","BLOCKSIZE");
    UT_OS_LOG_MACRO("\n--------------------------------------------------------------")
    UT_OS_LOG_MACRO("-------------------------------------------------------------\n")

    for (j=0; j < NUM_TABLE_ENTRIES; j++)
    {
        UT_OS_LOG_MACRO(" %-8s%-16s%-32s%-14s%-18d%-8d%-8d%-10d\n",
                OS_VolumeTable[j].DeviceName,
                OS_VolumeTable[j].PhysDevName,
                OS_VolumeTable[j].VolumeName,
                OS_VolumeTable[j].MountPoint,
                (int)OS_VolumeTable[j].VolatileFlag,
                (int)OS_VolumeTable[j].FreeFlag,
                (int)OS_VolumeTable[j].IsMounted,
                (int)OS_VolumeTable[j].BlockSize);
    }

    UT_OS_LOG_MACRO("\n")
#endif  /* UT_VERBOSE */
}


/*================================================================================*
** End of File: ut_os_stubs.c
**================================================================================*/
