#ifndef _ADCS_DRIVER_UTIL_H
#define _ADCS_DRIVER_UTIL_H

#include <time.h>
#include <sys/time.h>
#include <errno.h>
#include "stdint.h"

#define EPSILON (1E-9)

uint32_t get_micros_32(void);
uint64_t get_micros_64(void);
int msleep(uint32_t msec);
int dsleep(double seconds);
int nsleep(long nanoseconds);
double array_min(double *input, unsigned int count);
double array_max(double *input, unsigned int count);
double array_mean(double *input, unsigned int count);
double array_std(double *input, unsigned int count);
int min(int a, int b);
int max(int a, int b);
bool CompareDouble(double a, double b, double epsilon);


#endif /* _ADCS_DRIVER_UTIL_H */