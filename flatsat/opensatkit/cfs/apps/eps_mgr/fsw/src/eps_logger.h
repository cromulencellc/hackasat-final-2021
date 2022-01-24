
#ifndef _EPS_LOGGER_H_
#define _EPS_LOGGER_H_

#include <stdio.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>


#define LOG_LEVEL_NONE 0
#define LOG_LEVEL_ERROR 1
#define LOG_LEVEL_WARNING 2
#define LOG_LEVEL_INFO 3
#define LOG_LEVEL_DEBUG 4

// #define RED     "\033[31m"      /* Red */
// #define GREEN   "\033[32m"      /* Green */
// #define YELLOW  "\033[33m"      /* Yellow */
// #define BLUE    "\033[34m"      /* Blue */

#ifndef EPS_LOG_LEVEL
    #define EPS_LOG_LEVEL LOG_LEVEL_INFO
#endif

#ifdef EPS_LOG_COLOR
    #define LC_DEBUG "\33[34m"
    #define LC_INFO "\033[32m"
    #define LC_WARN "\033[33m"
    #define LC_ERROR "\033[31m"
    #define LC_END "\033[0m"
#endif


#if (EPS_LOG_LEVEL >= LOG_LEVEL_DEBUG)
    #ifdef EPS_LOG_COLOR
        #define EPS_LOG_DEBUG(str, ...) fprintf(stdout, LC_DEBUG "%s:DEBUG:%s:(%s:%d) " str LC_END "\n", getEPStime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define EPS_LOG_DEBUG(str, ...) fprintf(stdout, "%s:DEBUG:" "%s:(%s:%d) " str "\n", getEPStime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define EPS_LOG_DEBUG(str, ...)
#endif

#if (EPS_LOG_LEVEL >= LOG_LEVEL_INFO)
    #ifdef EPS_LOG_COLOR
        #define EPS_LOG_INFO(str, ...) fprintf(stdout, LC_INFO "%s:INFO:%s:(%s:%d) " str LC_END "\n", getEPStime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define EPS_LOG_INFO(str, ...) fprintf(stdout, "%s:INFO:%s:(%s:%d) " str "\n", getEPStime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define EPS_LOG_INFO(str, ...)
#endif

#if (EPS_LOG_LEVEL >= LOG_LEVEL_WARNING)
    #ifdef EPS_LOG_COLOR
        #define EPS_LOG_WARN(str, ...) fprintf(stdout, LC_WARN "%s:WARN:%s:(%s:%d) " str LC_END "\n", getEPStime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define EPS_LOG_WARN(str, ...) fprintf(stdout, "%s:WARN:%s:(%s:%d) " str "\n", getEPStime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define EPS_LOG_WARN(str, args...)
#endif

#if (EPS_LOG_LEVEL >= LOG_LEVEL_ERROR)
    #ifdef EPS_LOG_COLOR
        #define EPS_LOG_ERROR(str, ...) fprintf(stdout, LC_ERROR "%s:ERROR:%s:(%s:%d) " str LC_END "\n", getEPStime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #else
        #define EPS_LOG_ERROR(str, ...) fprintf(stdout, "%s:ERROR:%s:(%s:%d) " str "\n", getEPStime(), __func__, __FILE__, __LINE__, ##__VA_ARGS__)
    #endif
#else
    #define EPS_LOG_ERROR(str, ...)
#endif

static inline char* getEPStime(void) {
    static char tmbuf[32], fmt[32];
    static struct timeval curTime;
    struct tm * timeinfo;
    gettimeofday(&curTime, NULL);
    timeinfo = localtime(&curTime.tv_sec);
    strftime(fmt, sizeof(fmt), "%Y-%m-%d %H:%M:%S.%%03u", timeinfo);
    snprintf(tmbuf, sizeof(tmbuf), fmt, curTime.tv_usec / 1000);
    return tmbuf;
}

#endif
