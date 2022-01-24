/*
** Purpose: Define platform configurations for the Sla_tlm application
**
** Notes:
**   None
**
** License:
**   Template written by David McComas and licensed under the GNU
**   Lesser General Public License (LGPL).
**
** References:
**   1. OpenSatKit Object-based Application Developer's Guide.
**   2. cFS Application Developer's Guide.
**
*/

#ifndef _sla_tlm_platform_cfg_
#define _sla_tlm_platform_cfg_

/*
** Includes
*/

#include "sla_tlm_mission_cfg.h"
#include "sla_tlm_msgids.h"
#include "sla_tlm_perfids.h"

/******************************************************************************
** Application Macros
*/

#define  SLA_TLM_RUNLOOP_DELAY    500  /* Delay in milliseconds */

#define  SLA_TLM_CMD_PIPE_DEPTH    10
#define  SLA_TLM_CMD_PIPE_NAME     "SLA_TLM_CMD_PIPE"

/******************************************************************************
** Example Object Macros
*/

#define  SLA_TLM_EXTBL_DEF_LOAD_FILE  "/cf/sla_tlm_extbl.json"
#define  SLA_TLM_EXTBL_DEF_DUMP_FILE  "/cf/sla_tlm_extbl_d.json"

#endif /* _sla_tlm_platform_cfg_ */
