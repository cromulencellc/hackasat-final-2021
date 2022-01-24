
#ifndef _FLATSAT_FSW_
#define _FLATSAT_FSW_

#include "adcs_logger.h"
#ifdef _FLATSAT_STANDALONE_FSW_
#include "mathkit.h"
#include "dcmkit.h"
#include "orbkit.h"
#include "timekit.h"
#include "fswkit.h"
#include "AcTypes.h"
#else
#include "adcs_io_lib.h"
#include "orbkit.h"
#include "app_cfg.h"

#endif

#ifndef PI
   #define PI (3.141592653589793)
#endif

#ifndef TWOPI
   #define TWOPI (6.283185307179586)
#endif

#ifndef HALFPI
   #define HALFPI (1.570796326794897)
#endif

#ifndef D2R
   #define D2R (1.74532925199433E-2)
#endif

#ifndef R2D
    #define R2D (180.0/PI)
#endif

#ifndef RPM2RPS
    #define RPM2RPS ((TWOPI/60))
#endif

#ifndef RPS2RPM
    #define RPS2RPM (60/(TWOPI))
#endif

#ifndef TRUE
    #define TRUE 1
#endif

#ifndef FALSE
    #define FALSE 0
#endif

/* POV Frames */
#define FRAME_N 0
#define FRAME_L 1
#define FRAME_F 2

typedef enum fsw_attitude_knowledge_source_e {
    FSW_ATT_SOURCE_NONE = 0,
    FSW_ATT_SOURCE_RATES = 1,
    FSW_ATT_SOURCE_SUN_MAG = 2,
    FSW_ATT_SOURCE_ST = 3
} fsw_attitude_knowledge_source_t;

typedef enum fsw_wheel_ctrl_mode_e {
   FSW_CTRL_DISABLED = 0,
   FSW_CTRL_CLOSED_LOOP_LVLH = 1,
   FSW_CTRL_OPEN_LOOP_WHEEL = 2,
   FSW_CTRL_SUN_PAYLOAD_LVLH = 3,
   FSW_CTRL_RATE_ZERO = 4,
   FSW_CTRL_CLOSED_LOOP_SUN_TRACKING = 5
} fsw_wheel_ctrl_mode_t;

#define ADCS_MODE_CNT 5

typedef enum fsw_mtb_ctrl_mode_e {
    FSW_MTB_DISABLED = 0,
    FSW_MTB_MANUAL = 1,
    FSW_MTB_AUTO = 2
} fsw_mtb_ctrl_mode_t;

typedef enum adcs_calibration_e {
    ADCS_CAL_CSS = 0
} adcs_calibration_t;

void FlatSatFswAllocateAC(struct AcType *AC);
void FlatSatFswInitAC(struct AcType *AC);
int32_t FlatSatFsw(struct AcType *AC);
void FlatSatFswEnableDebug(struct AcType *AC);

#endif