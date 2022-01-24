#ifndef _XADC_DRIVER_H
#define _XADC_DRIVER_H

#include <stdio.h>
#include <limits.h>  
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <iio.h>
#include "adcs_driver_util.h"
#include "adcs_io_logger.h"

#define XADC_MAX_CHANNEL_COUNT 10
#define XADC_SAMPLE_RATE 50000
#define XADC_BUFFER_SIZE 1

typedef struct {
    uint16_t raw_value;
    // uint32_t channel_number;
    bool has_offset;
    uint32_t channel_index;
    double converted;
    double scale;
    double offset;
    char channel_name[20];
} xadc_channel_t;

typedef struct {
    bool active;
    bool use_buffer;
    uint8_t channel_count;
    uint16_t sample_rate;
    xadc_channel_t channels[XADC_MAX_CHANNEL_COUNT];
} xadc_device_t;


void xadc_constructor(xadc_device_t* device);
// bool xadc_init(const char* deviceName, unsigned int sampleRate);
bool xadc_init(const char* deviceName, const char* triggerName, unsigned int sampleRate, bool useBuffer);

bool xadc_setup_channels(unsigned int numChannels, const char **channelNames);
bool xadc_start_channels(const char* triggerName);
void xadc_update(void);
void xadc_set_channel_has_offset(unsigned int deviceChannelIdx, bool has_offset);
void xadc_stop_channels(void);
void xadc_shutdown(void);

#endif /* _XADC_DRIVER_H */