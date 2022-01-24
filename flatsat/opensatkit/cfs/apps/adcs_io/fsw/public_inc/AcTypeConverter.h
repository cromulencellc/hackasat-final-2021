

#ifndef __AC_TYPE_CONVERTER_H__
#define __AC_TYPE_CONVERTER_H__

#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "AcTypes.h"
#include "mathkit.h"
#include "dcmkit.h"
#include "fswkit.h"
// #include "iokit.h"
#include "timekit.h"

void StringToAcType(const char* Msg, size_t MsgLen, struct AcType *AC);
size_t AcTypeToString(struct AcType *AC, char* Msg);

#endif