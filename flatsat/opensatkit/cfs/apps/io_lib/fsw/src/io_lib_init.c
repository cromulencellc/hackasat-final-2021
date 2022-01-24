/*************************************************************************
* File: io_lib_init.c
**
** Copyright 2017 United States Government as represented by the Administrator
** of the National Aeronautics and Space Administration.  No copyright is
** claimed in the United States under Title 17, U.S. Code.
** All Other Rights Reserved.
**
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "io_lib.h"

#include "tmtf.h"
#include "tm_sync.h"
#include "tc_sync.h"

/*************************************************************************
** Macro Definitions
*************************************************************************/
IO_LIB_LibData_t    g_IO_LIB_LibData;


/*************************************************************************
** Protocol Library Init Functions
*************************************************************************/
extern int32 TMTF_LibInit(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* CFS Library Initialization Routine                              */
/* cFE requires that a library have an initialization routine      */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 IO_LibInit(void)
{
    int32 ii = 0;
    int32 iStatus = 0;

    /* Perform any library initializations */
    iStatus = TMTF_LibInit();
    iStatus |= TM_SYNC_LibInit(); 
    iStatus |= TC_SYNC_LibInit(); 


    // iStatus = CFE_ES_RegisterApp();
    if (iStatus != CFE_SUCCESS)
    {
        CFE_ES_WriteToSysLog("IO_Lib Init failed.\n", iStatus);
    } else {

        CFE_ES_WriteToSysLog("IO Lib Initialized.  Version %d.%d.%d.%d\n",
                IO_LIB_MAJOR_VERSION,
                IO_LIB_MINOR_VERSION, 
                IO_LIB_REVISION, 
                IO_LIB_MISSION_REV);

    }


               
    return iStatus;
 
}/* End CFS_LibInit */
