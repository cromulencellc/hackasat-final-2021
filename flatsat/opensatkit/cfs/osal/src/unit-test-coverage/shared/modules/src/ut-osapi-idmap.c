/* pull in the OSAL configuration */
#include "stub-map-to-real.h"
#include "osconfig.h"
#include "ut-osapi-idmap.h"

/*
 * Now include all extra logic required to stub-out subsequent calls to
 * library functions and replace with our own
 */
#include "osapi-idmap.c"

OS_SharedGlobalVars_t OS_SharedGlobalVars =
      {
            .Initialized = false
      };

int32 Osapi_Call_ObjectIdFindNext(uint32 idtype, uint32 *array_index, OS_common_record_t **record)
{
    return OS_ObjectIdFindNext(idtype, array_index, record);
}
