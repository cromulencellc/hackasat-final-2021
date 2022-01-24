/*
** Purpose: Define configurations for the application framework
**
** Notes:
**   1. Resources are statically allocated for each application's
**      data structures so these configurations muct be sized to 
**      accommodate the 'largest' application.
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

#ifndef _app_fw_platform_cfg_h_
#define _app_fw_platform_cfg_h_

#include "app_fw_mission_cfg.h"

/* 
** Mission specific version number
**
**  An application version number consists of four parts:
**  major version number, minor version number, revision
**  number and mission specific revision number. The mission
**  specific revision number is defined here and the other
**  parts are defined in "app_fw_version.h".
**
*/
#define APP_FW_LIB_MISSION_REV  (0)

#define  OSK_C_FW_ERROR      (0x0001)
#define  OSK_C_FW_CFS_ERROR  ((int32)(CFE_SEVERITY_ERROR | OSK_C_FW_ERROR))

/******************************************************************************
** Initialization Table (INITBL)
**
*/

#define  INITBL_MAX_CFG_ITEMS  45   /* Max number of JSON ini file configuration items */

/******************************************************************************
** Child Manager (CHILDMGR)
**
** Define child manager object parameters. The command buffer length must big
** enough to hold the largest command packet of all the apps using this utility.
*/

#define CHILDMGR_MAX_TASKS  5   /* Max number of child tasks registered for all apps */


#define CHILDMGR_CMD_BUFFER_LEN  256   /* Must be greater than largest cmd msg */ 
#define CHILDMGR_CMD_Q_ENTRIES     3   
#define CHILDMGR_CMD_FUNC_TOTAL   32

#endif /* _app_fw_platform_cfg_h_ */

/************************/
/*  End of File Comment */
/************************/
