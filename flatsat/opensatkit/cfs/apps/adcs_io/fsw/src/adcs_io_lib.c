/*************************************************************************
** File:
**   $Id: adcs_io_lib.c $
**
** Purpose: 
**   Adcs_io CFS library
**
*************************************************************************/

/*************************************************************************
** Includes
*************************************************************************/
#include "adcs_io_lib.h"
#include "adcs_io_lib_version.h"

/*************************************************************************
** Macro Definitions
*************************************************************************/


/*************************************************************************
** Private Function Prototypes
*************************************************************************/
int32 ADCS_IO_LibInit(void);

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Library Initialization Routine                                  */
/* cFE requires that a library have an initialization routine      */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 ADCS_IO_LibInit(void)
{
    
    OS_printf ("ADCS_IO Lib Initialized.  Version %d.%d.%d.%d\n",
                ADCS_IO_LIB_MAJOR_VERSION,
                ADCS_IO_LIB_MINOR_VERSION, 
                ADCS_IO_LIB_REVISION, 
                ADCS_IO_LIB_MISSION_REV);
                
    return CFE_SUCCESS;
 
}/* End ADCS_IO_LibInit */

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
/*                                                                 */
/* Adcs_io Lib function                                             */ 
/*                                                                 */
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int32 ADCS_IO_Function( void ) 
{
   OS_printf ("ADCS_IO_Function called\n");

   return(CFE_SUCCESS);
   
} /* End ADCS_IO_Function */

/************************/
/*  End of File Comment */
/************************/
