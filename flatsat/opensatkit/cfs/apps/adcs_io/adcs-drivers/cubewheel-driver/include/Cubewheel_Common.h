
#ifndef _CUBEWHEEL_COMMON_H
#define _CUBEWHEEL_COMMON_H


#ifndef PI
   #define PI (3.141592653589793)
#endif

#ifndef TWOPI
   #define TWOPI (6.283185307179586)
#endif

#ifndef RPM2RPS
    #define RPM2RPS ((TWOPI/60))
#endif

#ifndef RPS2RPM
    #define RPS2RPM (60/(TWOPI))
#endif


/**
 * @brief List of Valid Cubewheel Modes
 *  See Table 22
 * 
 * Reference CubeWheel - User Manual [v1.15]
 */
enum cubewheel_Mode_e {
    WHEEL_MODE_IDLE = 0, /**< @brief Wheel idle in this mode */
    WHEEL_MODE_NO_CONTROL = 1, /**< @brief No control mode  */
    WHEEL_MODE_DUTY_CYCLE_CONTROL = 2, /**< @brief Duty cycle input mode */
    WHEEL_MODE_USE_SPEED_CONTROL = 3 /**< @brief Speed controller mode */
} cubewheel_Mode_t;

#endif
