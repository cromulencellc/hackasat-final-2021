/*
**  GSC-18128-1, "Core Flight Executive Version 6.6"
**
**  Copyright (c) 2006-2019 United States Government as represented by
**  the Administrator of the National Aeronautics and Space Administration.
**  All Rights Reserved.
**
**  Licensed under the Apache License, Version 2.0 (the "License");
**  you may not use this file except in compliance with the License.
**  You may obtain a copy of the License at
**
**    http://www.apache.org/licenses/LICENSE-2.0
**
**  Unless required by applicable law or agreed to in writing, software
**  distributed under the License is distributed on an "AS IS" BASIS,
**  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
**  See the License for the specific language governing permissions and
**  limitations under the License.
*/

/******************************************************************************
** File: cfe_msgids.h
**
** Purpose:
**   This header file contains the Message Id's for messages used by the
**   cFE core.
**
** Author:   R.McGraw/SSI
**
** Notes:
**   This file should not contain messages defined by cFE external
**   applications.
**
******************************************************************************/
#ifndef _cfe_msgids_
#define _cfe_msgids_

/*
** Includes
*/

#include "cfe_mission_cfg.h"

/*
** cFE Command Message Id's
*/
#define CFE_EVS_CMD_MID         0x1812      /* 0x1812 */
                                                       /* Message ID 0x1802 is available  */
#define CFE_SB_CMD_MID          CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_SB_CMD_MSG          /* 0x1803 */
#define CFE_TBL_CMD_MID         CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TBL_CMD_MSG         /* 0x1804 */
#define CFE_TIME_CMD_MID        CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TIME_CMD_MSG        /* 0x1805 */
#define CFE_ES_CMD_MID          0x1826      /* 0x1826 */

#define CFE_ES_SEND_HK_MID      0x1828      /* 0x1828 */
#define CFE_EVS_SEND_HK_MID     0x1819      /* 0x1819 */
                                                       /* Message ID 0x180A is available  */
#define CFE_SB_SEND_HK_MID      CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_SB_SEND_HK_MSG      /* 0x180B */
#define CFE_TBL_SEND_HK_MID     CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TBL_SEND_HK_MSG     /* 0x180C */
#define CFE_TIME_SEND_HK_MID    CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TIME_SEND_HK_MSG    /* 0x180D */

#define CFE_TIME_TONE_CMD_MID   CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TIME_TONE_CMD_MSG   /* 0x1810 */
#define CFE_TIME_1HZ_CMD_MID    CFE_MISSION_CMD_MID_BASE1 + CFE_MISSION_TIME_1HZ_CMD_MSG    /* 0x1811 */


/*
** cFE Global Command Message Id's
*/
#define CFE_TIME_DATA_CMD_MID   CFE_MISSION_CMD_MID_BASE_GLOB + CFE_MISSION_TIME_DATA_CMD_MSG   /* 0x1860 */
#define CFE_TIME_SEND_CMD_MID   CFE_MISSION_CMD_MID_BASE_GLOB + CFE_MISSION_TIME_SEND_CMD_MSG   /* 0x1862 */


/*
** CFE Telemetry Message Id's
*/
#define CFE_ES_HK_TLM_MID           0x0820       /* 0x0820 */
#define CFE_EVS_HK_TLM_MID          0x0811       /* 0x0811 */
                                                       /* Message ID 0x0802 is available  */
#define CFE_SB_HK_TLM_MID           CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_SB_HK_TLM_MSG       /* 0x0803 */
#define CFE_TBL_HK_TLM_MID          CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_TBL_HK_TLM_MSG      /* 0x0804 */
#define CFE_TIME_HK_TLM_MID         CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_TIME_HK_TLM_MSG     /* 0x0805 */
#define CFE_TIME_DIAG_TLM_MID       CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_TIME_DIAG_TLM_MSG   /* 0x0806 */
#define CFE_EVS_LONG_EVENT_MSG_MID  0x0818   /* 0x0818 */
#define CFE_EVS_SHORT_EVENT_MSG_MID CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_EVS_SHORT_EVENT_MSG_MSG  /* 0x0809 */
#define CFE_SB_STATS_TLM_MID        CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_SB_STATS_TLM_MSG    /* 0x080A */
#define CFE_ES_APP_TLM_MID          0x082B      /* 0x082B */
#define CFE_TBL_REG_TLM_MID         CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_TBL_REG_TLM_MSG     /* 0x080C */
#define CFE_SB_ALLSUBS_TLM_MID      CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_SB_ALLSUBS_TLM_MSG  /* 0x080D */
#define CFE_SB_ONESUB_TLM_MID       CFE_MISSION_TLM_MID_BASE1 + CFE_MISSION_SB_ONESUB_TLM_MSG   /* 0x080E */
#define CFE_ES_SHELL_TLM_MID        0x082F    /* 0x081F */
#define CFE_ES_MEMSTATS_TLM_MID     0x0830    /* 0x0830 */

/*
 * MID definitions by these older names are required to make some existing apps compile
 * These are deprecated and should be removed after CFE 6.6
 */
#ifndef CFE_OMIT_DEPRECATED_6_6

#define CFE_EVS_EVENT_MSG_MID        CFE_EVS_LONG_EVENT_MSG_MID

#endif

#endif
