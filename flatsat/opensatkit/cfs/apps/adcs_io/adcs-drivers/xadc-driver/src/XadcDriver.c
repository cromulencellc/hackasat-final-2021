

#include "XadcDriver.h"
#include "adcs_driver_util.h"
#include "math.h"



// Streaming devices
static struct iio_device *xadc_device;
/* IIO structs required for streaming */
static struct iio_context *ctx;
static struct iio_buffer  *xadc_buf;
static struct iio_channel* channels[XADC_MAX_CHANNEL_COUNT];
static xadc_device_t *device;
uint16_t sample_buf[XADC_BUFFER_SIZE];
double converted_buf[XADC_BUFFER_SIZE];

/* 
* File level variables
*/
// static char tmpstr[16];

/**
 * Local Function Prototypes
 * 
*/
void samplesToVoltage(double *sampleScaled, uint16_t *sampleConverted, uint16_t sampleCount, double scale);
double mean(double *samples, unsigned int count);
// static char* get_ch_name(const char* type, int id);
// static ssize_t sample_cb(const struct iio_channel *chn,
//         void *buf, size_t len, void *d);
// static int attr_cb(struct iio_device *dev, const char *attr, const char *value, size_t len, void *d);


/*
* Local Cubewheel Prototypes
*/


/******************************************************************************
************************  EXPORTED FUNCTIONS  *********************************
******************************************************************************/

/******************************************************************************
** Function: cubewheel_constructor
**
*/
void xadc_constructor(xadc_device_t* inputDevice) {
    device = inputDevice;
    memset(device, 0, sizeof(xadc_device_t));
}

bool xadc_init(const char* deviceName, const char* triggerName, unsigned int sampleRate, bool useBuffer) {
    bool Status = true;
    // int i;
    struct iio_device *xadc_trigger;
    ctx = iio_create_local_context();
    device->sample_rate = sampleRate;
    device->use_buffer = useBuffer;
    if(ctx != NULL) {
        // int deviceCount = iio_context_get_devices_count(ctx);
        // DEBUG_PRINT("Found %d IIO Devices\n", deviceCount);
        // for(i = 0; i< deviceCount; i++) {
        //     xadc_device = iio_context_get_device(ctx, i);
        //     DEBUG_PRINT("Device Name %s\n", iio_device_get_name(xadc_device));
        // }
        xadc_device = iio_context_find_device(ctx, deviceName);
        if(iio_device_get_channels_count(xadc_device) == 14) {
            ADCS_IO_LOG_DEBUG("Found the right one\n");
        }
        // printf("Read device attributes:\n");
        // iio_device_attr_read_all(xadc_device, &attr_cb, NULL);
        // printf("Read buffer attributes:\n");
        // iio_device_buffer_attr_read_all(xadc_device, &attr_cb, NULL);
        if (xadc_device != NULL) {
            if(iio_device_attr_write_longlong(xadc_device, "sampling_frequency", device->sample_rate) != 0) {
                ADCS_IO_LOG_ERROR("Could not set XADC sample rate %d\n", device->sample_rate);
                Status = false;
            }

        } else {
            ADCS_IO_LOG_ERROR("Could not find XADC iio device %s\n", deviceName);
            Status = false;
        }
    } else {
        ADCS_IO_LOG_ERROR("Could not setup iio context\n");
        Status = false;

    }

    ADCS_IO_LOG_DEBUG("xadc_init() Status: %d\n", Status);
    if (triggerName != NULL) {
        xadc_trigger = iio_context_find_device(ctx, triggerName);
        if(xadc_trigger != NULL) {
            struct iio_device* temp_trigger;
            bool existingTrigger = false;
            if(iio_device_get_trigger(xadc_device, &temp_trigger) == 0) {
                if (temp_trigger != NULL) {
                    const char *existingTriggerName = iio_device_get_name(temp_trigger); 
                    if(strncmp(existingTriggerName, triggerName, strlen(triggerName)) != 0) {
                        ADCS_IO_LOG_ERROR("Existing trigger doesn't match desired trigger\n");
                        Status = false;
                    }
                    else {
                        existingTrigger = true;
                        ADCS_IO_LOG_DEBUG("Existing trigger matches desired trigger\n");
                    }
                } 

            }
            if(iio_device_set_trigger(xadc_device, xadc_trigger) != 0){
                if (!existingTrigger) {
                    ADCS_IO_LOG_ERROR("Could not set XADC trigger %s\n", triggerName);
                    Status = false;
                }
            }
        } else {
            ADCS_IO_LOG_ERROR("Could not find XADC trigger device %s\n", triggerName);
            Status = false;
        }
    }


    return Status;
}

bool xadc_setup_channels(unsigned int numChannels, const char **channelNames) {
    int i;
    bool Status = true;
    struct iio_channel *chn; 

    if(Status) {
        device->channel_count = numChannels;
        if (xadc_device != NULL) {
            for (i = 0; i<device->channel_count; i++) {
                // device->channels[i].channel_number = inputChannels[i];
                snprintf(device->channels[i].channel_name, sizeof(device->channels[i].channel_name), "%s", channelNames[i]);
                chn = iio_device_find_channel(xadc_device, channelNames[i], false);
                if (chn != NULL) {
                    device->channels[i].channel_index = iio_channel_get_index(chn);
                    ADCS_IO_LOG_DEBUG("Channel Found %s\n", iio_channel_get_id(chn));
                    ADCS_IO_LOG_DEBUG("Channel Index: %d\n", device->channels[i].channel_index);
                }
            }
            // channels = calloc(device->channel_count, sizeof(*channels));
            if (channels == NULL) {
                ADCS_IO_LOG_ERROR("Channel array allocation failed");
                Status = false;
            }
        }


    }

    int channelEnabled = 0;
    int j;
    bool desiredChannel;
    double tempVal = 0;
    if(xadc_device != NULL && Status) {
        unsigned int deviceChannelCount = iio_device_get_channels_count(xadc_device);
        for (i = 0; i<deviceChannelCount; i++) {
            // struct iio_channel *chn = iio_device_get_channel(xadc_device, device->channels[i].channel_index);
            chn = iio_device_get_channel(xadc_device, i);
            desiredChannel = false;

            if (chn != NULL) {
                long id = iio_channel_get_index(chn);
                for (j=0; j<device->channel_count; j++) {
                    if(id == device->channels[j].channel_index) {
                        desiredChannel = true;
                        if (iio_channel_is_scan_element(chn)) {
                            ADCS_IO_LOG_DEBUG("Enable Desired Channel Name %s, Index %ld\n", iio_channel_get_id(chn), id);
                            channels[j] = chn;
                            int ret = iio_channel_attr_read_double(channels[j], "scale", &tempVal);
                            if (ret == 0) {
                                device->channels[j].scale = tempVal;

                            } else {
                                device->channels[j].scale = 0.0;
                                ADCS_IO_LOG_ERROR("Failed to read 'scale' for channel %s\n", iio_channel_get_id(chn));
                            }
                            if (device->channels[j].has_offset) {
                                ret = iio_channel_attr_read_double(channels[j], "offset", &tempVal);
                                if (ret == 0) {
                                    device->channels[j].offset = tempVal;

                                } else {
                                    device->channels[j].offset = 0.0;
                                    ADCS_IO_LOG_ERROR("Failed to read 'offset' for channel %s\n", iio_channel_get_id(chn));
                                }
                            }
                            iio_channel_enable(chn);
                            channelEnabled++;
                        } 
                        break;
                    }
                }
                if (!desiredChannel) {
                    ADCS_IO_LOG_DEBUG("Disable Channel Name %s, Index %ld\n", iio_channel_get_id(chn), id);
                    iio_channel_disable(chn);
                }
            } else {
                ADCS_IO_LOG_ERROR("iio could not find channel index %d\n",  device->channels[i].channel_index);
            }
        }
    } else {
        ADCS_IO_LOG_ERROR("Channel setup failed, no IIO XADC device\n");
    }

    if (channelEnabled == device->channel_count) {
        if (device->use_buffer) {
            xadc_buf = iio_device_create_buffer(xadc_device, XADC_BUFFER_SIZE, 0);
            if (xadc_buf != NULL) {
                device->active = true;
            } else {
                ADCS_IO_LOG_ERROR("Could not create iio data buffer\n");
                Status = false;
            }
        } else {

            device->active = true;
        }
    }
    ADCS_IO_LOG_DEBUG("xadc_start_channels() Status: %d, Device Enabled %d\n", Status, device->active);

    return Status;

}

// bool xadc_start_channels(const char* triggerName) {
//     int i;

//     return Status;
// }

void xadc_stop_channels(void) {
    int i;
    if (xadc_buf != NULL) {
        // iio_buffer_cancel(xadc_buf);
        iio_buffer_destroy(xadc_buf);
        xadc_buf = NULL;
    }
    for (i = 0; i<device->channel_count; i++) {
        if (channels[i] != NULL) {
            iio_channel_disable(channels[i]);
        } else {
            ADCS_IO_LOG_ERROR("iio could not disable channel index %d, name %s\n", device->channels[i].channel_index, device->channels[i].channel_name);
        }
    }
    device->active = false;
}

void xadc_shutdown(void) {
    xadc_stop_channels();
    // if (channels) { free(channels); }
    if(!iio_device_set_trigger(xadc_device, NULL)) {
        ADCS_IO_LOG_DEBUG("Disassociate trigger failure");
    }
    if(ctx != NULL) {
        iio_context_destroy(ctx);
    }
}

void xadc_update(void) {
    int i;

    if(device->active) {
        
        // xadc_nbytes = iio_buffer_foreach_sample(xadc_buf, &sample_cb, NULL);
        if (device->use_buffer) {
            ssize_t xadc_nbytes;
            size_t channel_nbytes;
            xadc_nbytes = iio_buffer_refill(xadc_buf);        
            ADCS_IO_LOG_DEBUG("XADC Bytes Read to Device Buffer %d\n", xadc_nbytes);
            if (xadc_nbytes > 0) {
                for (i = 0; i<device->channel_count; i++) {
                    if(channels[i] != NULL) {

                        ADCS_IO_LOG_DEBUG("Channel Name %s, Idx %ld\n", iio_channel_get_id(channels[i]), iio_channel_get_index(channels[i]));
                        ADCS_IO_LOG_DEBUG("XADC Channel Enabled %d\n", iio_channel_is_enabled(channels[i]));
                        const struct iio_data_format *channel_format = iio_channel_get_data_format(channels[i]);
                        // unsigned int repeat = has_repeat ? channel_format->repeat : 1;
                        size_t sample_size = channel_format->length / 8 * channel_format->repeat;
                        channel_nbytes = iio_channel_read(channels[i], xadc_buf, sample_buf, XADC_BUFFER_SIZE * sample_size);
                        if (channel_nbytes == 0) {
                            ADCS_IO_LOG_WARN("XADCS Channel Bytes == 0");
                        }
                        ADCS_IO_LOG_DEBUG("XADC Channel Bytes Processed %d\n", channel_nbytes);
                        samplesToVoltage(converted_buf, sample_buf, XADC_BUFFER_SIZE, channel_format->scale);
                        device->channels[i].converted = mean(converted_buf, XADC_BUFFER_SIZE);
                        ADCS_IO_LOG_DEBUG("Channel %s converted value = %f\n", device->channels[i].channel_name, device->channels[i].converted);
                    }
                }
            }
        } else {
            int ret;
            long long tempSample = 0;
            for (i = 0; i<device->channel_count; i++) {
                // iio_channel_attr_read_all(channels[i], &attr_cb, NULL);
                ret = iio_channel_attr_read_longlong(channels[i], "raw", &tempSample );
                if (ret != 0) {
                    ADCS_IO_LOG_ERROR("Failed to read 'raw value' for channel %s\n", iio_channel_get_id(channels[i]));
                }
                device->channels[i].raw_value = tempSample;
                device->channels[i].converted = (((double) tempSample) + device->channels[i].offset) * device->channels[i].scale;
                ADCS_IO_LOG_DEBUG("Channel %s converted output = %f\n", device->channels[i].channel_name, device->channels[i].converted);

            }
        }

        
    }
}

void xadc_set_channel_has_offset(unsigned int deviceChannelIdx, bool has_offset) {
    if (device->channels[deviceChannelIdx].channel_name != NULL) {
        device->channels[deviceChannelIdx].has_offset = has_offset;
    }
}

/**
 * Local Function Prototypes
*/

/* helper function generating channel names */
// static char* get_ch_name(const char* type, int id)
// {
//     memset(tmpstr, 0, sizeof(tmpstr));
//     snprintf(tmpstr, sizeof(tmpstr), "%s%d", type, id);
//     return tmpstr;
// }

void samplesToVoltage(double *sampleScaled, uint16_t *sampleConverted, uint16_t sampleCount, double scale) {
    for (int i=0; i<sampleCount; i++) {
        sampleScaled[i] = (double) sampleConverted[i] * scale;
    }
}

double mean(double *samples, unsigned int count) {
    double output = 0;
    for (int i = 0; i<count; i++) {
        output += samples[i];
    }
    return output / ((double) count);
}

// static ssize_t sample_cb(const struct iio_channel *chn, void *buf, size_t len, void *d)
// {
//     int idx = 0;
//     ADCS_IO_LOG_DEBUG("Channel Name %s, Idx %ld, Len %d, Data 0x%04X\n", iio_channel_get_id(chn), iio_channel_get_index(chn), len , *((uint16_t *)buf));
//     return (ssize_t) len;
// }

// static int attr_cb(struct iio_device *dev, const char *attr, const char *value, size_t len, void *d) {
//     ADCS_IO_LOG_DEBUG("Retrieved attribute %s. Value %s\n", attr, value);
// }