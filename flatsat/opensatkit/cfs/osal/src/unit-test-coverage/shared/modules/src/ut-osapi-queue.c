/* pull in the OSAL configuration */
#include "stub-map-to-real.h"
#include "osconfig.h"
#include "ut-osapi-queue.h"

/*
 * Now include all extra logic required to stub-out subsequent calls to
 * library functions and replace with our own
 */
#include "osapi-queue.c"

