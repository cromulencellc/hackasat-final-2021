
#ifndef _PDB_LOGGER_H_
#define _PDB_LOGGER_H_

#include "Arduino.h"
#include "app_cfg.h"
#include <stdio.h>

#ifndef SERIAL_OUTPUT
    #define SERIAL_OUTPUT Serial
#endif
// #include <string.h>
// #include "TimeLib.h"

#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4


// #define RED     "\033[31m"      /* Red */
// #define GREEN   "\033[32m"      /* Green */
// #define YELLOW  "\033[33m"      /* Yellow */
// #define BLUE    "\033[34m"      /* Blue */

#ifndef PDB_LOG_LEVEL
    #define PDB_LOG_LEVEL LOG_LEVEL_INFO
#endif

#ifdef PDB_LOG_COLOR
    #define LC_DEBUG "\33[34m"
    #define LC_INFO "\033[32m"
    #define LC_WARN "\033[33m"
    #define LC_ERROR "\033[31m"
    #define LC_END "\033[0m"
#endif


#if (PDB_LOG_LEVEL >= LOG_LEVEL_DEBUG)
    #ifdef PDB_LOG_COLOR
        #define LOG_DEBUG(str, ...) SERIAL_OUTPUT.printf( LC_DEBUG "%sDEBUG:%s:(%s:%d) " str LC_END "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define LOG_DEBUG(str, ...) SERIAL_OUTPUT.printf( "%sDEBUG:" "%s(%s:%d) " str "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define LOG_DEBUG(str, ...)
#endif

#if (PDB_LOG_LEVEL >= LOG_LEVEL_INFO)
    #ifdef PDB_LOG_COLOR
        #define LOG_INFO(str, ...) SERIAL_OUTPUT.printf( LC_INFO "%sINFO:%s:(%s:%d) " str LC_END "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define LOG_INFO(str, ...) SERIAL_OUTPUT.printf( "%sINFO:%s:(%s:%d) " str "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define LOG_INFO(str, ...)
#endif

#if (PDB_LOG_LEVEL >= LOG_LEVEL_WARNING)
    #ifdef PDB_LOG_COLOR
        #define LOG_WARN(str, ...) SERIAL_OUTPUT.printf( LC_WARN "%sWARN:%s:(%s:%d) " str LC_END "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define LOG_WARN(str, ...) SERIAL_OUTPUT.printf( "%sWARN:%s:(%s:%d) " str "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define LOG_WARN(str, args...)
#endif

#if (PDB_LOG_LEVEL >= LOG_LEVEL_ERROR)
    #ifdef PDB_LOG_COLOR
        #define LOG_ERROR(str, ...) SERIAL_OUTPUT.printf( LC_ERROR "%sERROR:%s:(%s:%d) " str LC_END "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define LOG_ERROR(str, ...) SERIAL_OUTPUT.printf( "%sERROR:%s:(%s:%d) " str "\n", gettime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define LOG_ERROR(str, ...)
#endif


static inline char* gettime(void) {
    static char tmbuf[32];

    return tmbuf;
}

#endif