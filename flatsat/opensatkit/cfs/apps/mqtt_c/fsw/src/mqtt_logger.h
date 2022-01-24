
#ifndef _MQTT_LOGGER_H_
#define _MQTT_LOGGER_H_

#include <stdio.h>
#include <string.h>
#include "cfe.h"

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4

// #define RED     "\033[31m"      /* Red */
// #define GREEN   "\033[32m"      /* Green */
// #define YELLOW  "\033[33m"      /* Yellow */
// #define BLUE    "\033[34m"      /* Blue */

#ifndef MQTT_LOG_LEVEL
    #define MQTT_LOG_LEVEL LOG_LEVEL_INFO
#endif

#ifdef MQTT_LOG_COLOR
    #define LC_DEBUG "\33[34m"
    #define LC_INFO "\033[32m"
    #define LC_WARN "\033[33m"
    #define LC_ERROR "\033[31m"
    #define LC_END "\033[0m"
#endif


#if (MQTT_LOG_LEVEL >= LOG_LEVEL_DEBUG)
    #ifdef MQTT_LOG_COLOR
        #define MQTT_LOG_DEBUG(str, ...) printf(LC_DEBUG "%s:DEBUG:%s:(%s:%d) " str LC_END "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define MQTT_LOG_DEBUG(str, ...) printf("%s:DEBUG:" "%s:(%s:%d) " str "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define MQTT_LOG_DEBUG(str, ...)
#endif

#if (MQTT_LOG_LEVEL >= LOG_LEVEL_INFO)
    #ifdef MQTT_LOG_COLOR
        #define MQTT_LOG_INFO(str, ...) printf(LC_INFO "%s:INFO:%s:(%s:%d) " str LC_END "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define MQTT_LOG_INFO(str, ...) printf("%s:INFO:%s:(%s:%d) " str "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define MQTT_LOG_INFO(str, ...)
#endif

#if (MQTT_LOG_LEVEL >= LOG_LEVEL_WARNING)
    #ifdef MQTT_LOG_COLOR
        #define MQTT_LOG_WARN(str, ...) printf(LC_WARN "%s:WARN:%s:(%s:%d) " str LC_END "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define MQTT_LOG_WARN(str, ...) printf("%s:WARN:%s:(%s:%d) " str "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define MQTT_LOG_WARN(str, args...)
#endif

#if (MQTT_LOG_LEVEL >= LOG_LEVEL_ERROR)
    #ifdef MQTT_LOG_COLOR
        #define MQTT_LOG_ERROR(str, ...) printf(LC_ERROR "%s:ERROR:%s:(%s:%d) " str LC_END "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define MQTT_LOG_ERROR(str, ...) printf("%s:ERROR:%s:(%s:%d) " str "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define MQTT_LOG_ERROR(str, ...)
#endif


static inline char* gettime(void) {
    static char tmbuf[32];
    CFE_TIME_Print(tmbuf, CFE_TIME_GetTime());
    return tmbuf;
}

#endif
