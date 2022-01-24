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

/************************************************************************************************
** File:  cfe_psp_watchdog.c
**
** Purpose:
**   This file contains glue routines between the cFE and the OS Board Support Package ( BSP ).
**   The functions here allow the cFE to interface functions that are board and OS specific
**   and usually dont fit well in the OS abstraction layer.
**
** History:
**   2009/07/20  A. Cudmore    | Initial version,
**
*************************************************************************************************/

/*
**  Include Files
*/


/*
** cFE includes
*/
#include "common_types.h"
#include "osapi.h"

/*
**  System Include Files
*/
#include <stdio.h>
#include <stdlib.h>

#include <rtems.h>
#include <bsp.h> /* for device driver prototypes */
#include <bsp/watchdog.h>

/*
** Types and prototypes for this module
*/
#include "cfe_psp.h"
#include "cfe_psp_config.h"

/*
** Global data
*/

/*
** The watchdog time in milliseconds
*/
uint32 CFE_PSP_WatchdogValue = 0;

/*  Function:  CFE_PSP_WatchdogInit()
**
**  Purpose:
**    To setup the timer resolution and/or other settings custom to this platform.
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_WatchdogInit(void)
{

   /*
   ** Just set it to a value right now
   ** The pc-linux desktop platform does not actually implement a watchdog
   ** timeout ( but could with a signal )
   */
   CFE_PSP_WatchdogValue = CFE_PSP_WATCHDOG_MAX;

}

static void writeTimerStatus(){

  volatile int *gptimer0 = (int*)0x80000300;
  volatile int scalar, scalar_rld, cfg, latch_cfg, counter1, reload1, ctrl1, latch1;
  volatile int counter2, reload2, ctrl2, latch2;


  scalar = gptimer0[0];
  scalar_rld = gptimer0[1];
  cfg = gptimer0[2];
  latch_cfg = gptimer0[3];
  counter1 = gptimer0[4];
  reload1 = gptimer0[5];
  ctrl1 = gptimer0[6];
  latch1 = gptimer0[7];
  counter2 = gptimer0[8];
  reload2 = gptimer0[9];
  ctrl2 = gptimer0[10];
  latch2 = gptimer0[11];

  printf("gptimer0 scalar: 0x%08X\n", scalar);
  printf("gptimer0 scalar reload: 0x%08X\n", scalar_rld);
  printf("gptimer0 cfg: 0x%08X\n", cfg);
  printf("gptimer0 latch_cfg: 0x%08X\n", latch_cfg);
  printf("gptimer0 counter1: 0x%08X\n", counter1);
  printf("gptimer0 reload1: 0x%08X\n", reload1);
  printf("gptimer0 ctrl1: 0x%08X\n", ctrl1);
  printf("gptimer0 latch1: 0x%08X\n", latch1);
  printf("gptimer0 counter2: 0x%08X\n", counter2);
  printf("gptimer0 reload2: 0x%08X\n", reload2);
  printf("gptimer0 ctrl2: 0x%08X\n", ctrl2);
  printf("gptimer0 latch2: 0x%08X\n", latch2);
}


/******************************************************************************
**  Function:  CFE_PSP_WatchdogEnable()
**
**  Purpose:
**    Enable the watchdog timer
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_WatchdogEnable(void)
{

}


/******************************************************************************
**  Function:  CFE_PSP_WatchdogDisable()
**
**  Purpose:
**    Disable the watchdog timer
**
**  Arguments:
**
**  Return:
*/
void CFE_PSP_WatchdogDisable(void)
{

}

/******************************************************************************
**  Function:  CFE_PSP_WatchdogService()
**
**  Purpose:
**    Load the watchdog timer with a count that corresponds to the millisecond
**    time given in the parameter.
**
**  Arguments:
**    None.
**
**  Return:
**    None
**
**  Notes:
**
*/
void CFE_PSP_WatchdogService(void)
{

   // uint32    ticks_per_second;
   uint32    watchdog_ticks;

   watchdog_ticks = 0x989680;
   // OS_printf("watchdog ticks %08X\n", watchdog_ticks);
   // writeTimerStatus();

   bsp_watchdog_reload(0, watchdog_ticks);  /* Calc time needed depending on work required */

}

/******************************************************************************
**  Function:  CFE_PSP_WatchdogGet
**
**  Purpose:
**    Get the current watchdog value. 
**
**  Arguments:
**    none 
**
**  Return:
**    the current watchdog value 
**
**  Notes:
**
*/
uint32 CFE_PSP_WatchdogGet(void)
{
   return(CFE_PSP_WatchdogValue);
}


/******************************************************************************
**  Function:  CFE_PSP_WatchdogSet
**
**  Purpose:
**    Get the current watchdog value. 
**
**  Arguments:
**    The new watchdog value 
**
**  Return:
**    nothing 
**
**  Notes:
**
*/
void CFE_PSP_WatchdogSet(uint32 WatchdogValue)
{

    CFE_PSP_WatchdogValue = WatchdogValue;

}

