#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <math.h>
#include "adcs_driver_util.h"
#include "adcs_io_logger.h"

/******************************************************************************
** Function: get_micros_64
**
*/
uint64_t get_micros_64(void) {
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    uint64_t micro = (uint64_t) (curTime.tv_sec*(uint64_t)1000000+curTime.tv_usec);
    // DEBUG_PRINT("Return time %ud\n", micro);
    return micro % __UINT64_MAX__;
}

/******************************************************************************
** Function: get_micros_32
**
*/
uint32_t get_micros_32(void) {
    struct timeval curTime;
    gettimeofday(&curTime, NULL);
    uint32_t micro = (uint32_t) ((curTime.tv_sec*(uint64_t)1000000+curTime.tv_usec));
    // DEBUG_PRINT("Return time %ud\n", micro);
    return micro % __UINT32_MAX__;
}


/******************************************************************************
** Function: dsleep
**
*/
int dsleep(double seconds)
{
    struct timespec ts;
    int res;

    if (seconds < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = (long) seconds;
    ts.tv_nsec = (long) ((seconds - ts.tv_sec) * 1e9);
    ADCS_IO_LOG_DEBUG("Sleep %12.9f sec", seconds);
    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

/******************************************************************************
** Function: msleep
**
*/
int msleep(uint32_t msec)
{
    struct timespec ts;
    int res;

    if (msec < 0)
    {
        errno = EINVAL;
        return -1;
    }

    ts.tv_sec = msec / 1000;
    ts.tv_nsec = (msec % 1000) * 1000000;
    ADCS_IO_LOG_DEBUG("Sleep %ld:%ld\n", ts.tv_sec, ts.tv_nsec);
    do {
        res = nanosleep(&ts, &ts);
    } while (res && errno == EINTR);

    return res;
}

/******************************************************************************
** Function: nsleep
**
*/
int nsleep(long nanoseconds)
{
   struct timespec req, rem;

   if(nanoseconds > 999)
   {   
        req.tv_sec = (int)(nanoseconds / 1000);                            /* Must be Non-Negative */
        req.tv_nsec = (nanoseconds - ((long)req.tv_sec * 1000)) * 1000000; /* Must be in range of 0 to 999999999 */
   }   
   else
   {   
        req.tv_sec = 0;                         /* Must be Non-Negative */
        req.tv_nsec = nanoseconds * 1000000;    /* Must be in range of 0 to 999999999 */
   }   

   return nanosleep(&req , &rem);
}

/******************************************************************************
** Function: array_min
**
*/
double array_min(double *input, unsigned int count) {
    double minV = input[0];
    for(int i=0; i<count; i++) {
        if (input[i] < minV) {
            minV = input[i];
        }
    }
    return minV;
}

/******************************************************************************
** Function: array_min
**
*/
double array_max(double *input, unsigned int count) {
    double maxV = input[0];
    for(int i=0; i<count; i++) {
        if (input[i] > maxV) {
            maxV = input[i];
        }
    }
    return maxV;
}

/******************************************************************************
** Function: array_mean
**
*/
double array_mean(double *input, unsigned int count) {
    double meanV = 0;
    for(int i=0; i<count; i++) {
        meanV += input[i];
    }
    return meanV/count;
}

/******************************************************************************
** Function: array_std
**
*/
double array_std(double *input, unsigned int count) {
    double meanV = array_mean(input, count);
    double stdV = 0;
    double tempV;
    for(int i=0; i<count; i++) {
        tempV = input[i] - meanV;
        stdV = tempV * tempV;
    }
    return sqrt(stdV/count);
}

/******************************************************************************
** Function: min
**
*/
int min(int a, int b) {
    return ((a) < (b) ? (a) : (b));
}

/******************************************************************************
** Function: max
**
*/
int max(int a, int b) {
    return ((a) > (b) ? (a) : (b));
}

/******************************************************************************
** Function: CompareDouble
**
*/
bool CompareDouble(double a, double b, double epsilon)
{
    return fabs(a - b) < epsilon;
}