/*************************************************************************
** File: adcs_io_lib.h
**
** Purpose: 
**   Specification for the adcs_io library functions.
**
*************************************************************************/
#ifndef _adcs_io_lib_h_
#define _adcs_io_lib_h_

/************************************************************************
** Includes
*************************************************************************/
#include "cfe.h"
#include "TorqueRod.h"
#include "BNO08x_Driver.h"
#include "gpio_device.h"
#include "Cubewheel.h"
#include "XadcDriver.h"
#include <stdlib.h>
#include <math.h>
#include "AcTypes.h"
#include "mathkit.h"
#include "dcmkit.h"
#include "fswkit.h"
#include "timekit.h"
#include "AcTypeConverter.h"
/************************************************************************
** Type Definitions
*************************************************************************/
#define AC_TYPE_SIZE sizeof(struct AcType)

/*************************************************************************
** Exported Functions
*************************************************************************/
/************************************************************************/
/** \brief Adcs_io Lib Function 
**  
**  \par Description
**        This is a adcs_io function
**
**  \par Assumptions, External Events, and Notes:
**        None
**       
**  \returns
**  \retstmt Returns #CFE_SUCCESS \endcode
**  \endreturns
** 
*************************************************************************/
int32 ADCS_IO_Function( void ); 

#endif /* _adcs_io_lib_h_ */

/************************/
/*  End of File Comment */
/************************/
