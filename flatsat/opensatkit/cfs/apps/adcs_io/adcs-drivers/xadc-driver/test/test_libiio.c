
/*
 * libiio - Dummy IIO streaming example
 *
 * This example libiio program is meant to exercise the features of IIO present
 * in the sample dummy IIO device. For buffered access it relies on the hrtimer
 * trigger but could be modified to use the sysfs trigger. No hardware should
 * be required to run this program.
 *
 * Copyright (c) 2016, DAQRI. All rights reserved.
 * Author: Lucas Magasweran <lucas.magasweran@daqri.com>
 *
 * Based on AD9361 example:
 * Copyright (C) 2014 IABG mbH
 * Author: Michael Feilen <feilen_at_iabg.de>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */
/*
 * How to setup the sample IIO dummy device and hrtimer trigger:
 *
 * 1. Check if `configfs` is already mounted
 *
 * $ mount | grep 'config'
 * configfs on /sys/kernel/config type configfs (rw,relatime)
 *
 * 1.b. Mount `configfs` if it is not already mounted
 *  $ sudo mount -t configfs none /sys/kernel/config
 *
 * 2. Load modules one by one
 *
 * $ sudo modprobe industrialio
 * $ sudo modprobe industrialio-configfs
 * $ sudo modprobe industrialio-sw-device
 * $ sudo modprobe industrialio-sw-trigger
 * $ sudo modprobe iio-trig-hrtimer
 * $ sudo modprobe iio_dummy
 *
 * 3. Create trigger and dummy device under `/sys/kernel/config`
 *
 * $ sudo mkdir /sys/kernel/config/iio/triggers/hrtimer/instance1
 * $ sudo mkdir /sys/kernel/config/iio/devices/dummy/my_dummy_device
 *
 * 4. Run `iio_info` to see that all worked properly
 *
 * $ iio_info
 * Library version: 0.14 (git tag: c9909f2)
 * Compiled with backends: local xml ip
 * IIO context created with local backend.
 * Backend version: 0.14 (git tag: c9909f2)
 * Backend description string: Linux ...
 * IIO context has 1 attributes:
 *         local,kernel: 4.13.0-39-generic
 * IIO context has 2 devices:
 *         iio:device0: my_dummy_device
 *                 10 channels found:
 *                         activity_walking:  (input)
 *                         1 channel-specific attributes found:
 *                                 attr  0: input value: 4
 * ...
 *
 **/
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <stdio.h>
#include <errno.h>
#include <getopt.h>
#include <inttypes.h>
#include <iio.h>
#define ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#define IIO_ENSURE(expr) { \
    if (!(expr)) { \
        (void) fprintf(stderr, "assertion failed (%s:%d)\n", __FILE__, __LINE__); \
        (void) abort(); \
    } \
}
static char *name        = "iio_dummy_part_no";
static char *trigger_str = "instance1";
static int buffer_length = 1;
static int count         = -1;
// libiio supports multiple methods for reading data from a buffer
enum {
    BUFFER_POINTER,
    SAMPLE_CALLBACK,
    CHANNEL_READ_RAW,
    CHANNEL_READ,
    MAX_READ_METHOD,
};
static int buffer_read_method = BUFFER_POINTER;
// Streaming devices
static struct iio_device *dev;
/* IIO structs required for streaming */
static struct iio_context *ctx;
static struct iio_buffer  *rxbuf;
static struct iio_channel **channels;
static unsigned int channel_count;
static bool stop;
static bool has_repeat;
/* cleanup and exit */
static void shutdown()
{
    int ret;
    if (channels) { free(channels); }
    printf("* Destroying buffers\n");
    if (rxbuf) { iio_buffer_destroy(rxbuf); }
    printf("* Disassociate trigger\n");
    if (dev) {
        ret = iio_device_set_trigger(dev, NULL);
        if (ret < 0) {
            char buf[256];
            iio_strerror(-ret, buf, sizeof(buf));
            fprintf(stderr, "%s (%d) while Disassociate trigger\n", buf, ret);
        }
    }
    printf("* Destroying context\n");
    if (ctx) { iio_context_destroy(ctx); }
    exit(0);
}
static void handle_sig(int sig)
{
    printf("Waiting for process to finish... got signal : %d\n", sig);
    stop = true;
}
static ssize_t sample_cb(const struct iio_channel *chn, void *src, size_t bytes, __notused void *d)
{
    const struct iio_data_format *fmt = iio_channel_get_data_format(chn);
    unsigned int j, repeat = has_repeat ? fmt->repeat : 1;
    printf("%s ", iio_channel_get_id(chn));
    for (j = 0; j < repeat; ++j) {
        if (bytes == sizeof(int16_t))
            printf("%" PRIi16 " ", ((int16_t *)src)[j]);
        else if (bytes == sizeof(int64_t))
            printf("%" PRId64 " ", ((int64_t *)src)[j]);
    }
    return bytes * repeat;
}
static void usage(__notused int argc, char *argv[])
{
    printf("Usage: %s [OPTION]\n", argv[0]);
    printf("  -d\tdevice name (default \"iio_dummy_part_no\")\n");
    printf("  -t\ttrigger name (default \"instance1\")\n");
    printf("  -b\tbuffer length (default 1)\n");
    printf("  -r\tread method (default 0 pointer, 1 callback, 2 read raw, 3 read)\n");
    printf("  -c\tread count (default no limit)\n");
}
static void parse_options(int argc, char *argv[])
{
    int c;
    while ((c = getopt(argc, argv, "d:t:b:r:c:h")) != -1) {
        switch (c)
        {
        case 'd':
            name = optarg;
            break;
        case 't':
            trigger_str = optarg;
            break;
        case 'b':
            buffer_length = atoi(optarg);
            break;
        case 'r':
            if (atoi(optarg) >= 0 && atoi(optarg) < MAX_READ_METHOD) {
                buffer_read_method = atoi(optarg);
            } else {
                usage(argc, argv);
                exit(1);
            }
            break;
        case 'c':
            if (atoi(optarg) > 0) {
                count = atoi(optarg);
            } else {
                usage(argc, argv);
                exit(1);
            }
            break;
        case 'h':
        default:
            usage(argc, argv);
            exit(1);
        }
    }
}
/* simple configuration and streaming */
int main (int argc, char **argv)
{
    // Hardware trigger
    struct iio_device *trigger;
    parse_options(argc, argv);
    // Listen to ctrl+c and assert
    signal(SIGINT, handle_sig);
    unsigned int i, j, major, minor;
    char git_tag[8];
    iio_library_get_version(&major, &minor, git_tag);
    printf("Library version: %u.%u (git tag: %s)\n", major, minor, git_tag);
    /* check for struct iio_data_format.repeat support
     * 0.8 has repeat support, so anything greater than that */
    has_repeat = ((major * 10000) + minor) >= 8 ? true : false;
    printf("* Acquiring IIO context\n");
    IIO_ENSURE((ctx = iio_create_default_context()) && "No context");
    IIO_ENSURE(iio_context_get_devices_count(ctx) > 0 && "No devices");
    printf("* Acquiring device %s\n", name);
    dev = iio_context_find_device(ctx, name);
    if (!dev) {
        perror("No device found");
        shutdown();
    }
    printf("* Initializing IIO streaming channels:\n");
    for (i = 0; i < iio_device_get_channels_count(dev); ++i) {
        struct iio_channel *chn = iio_device_get_channel(dev, i);
        if (iio_channel_is_scan_element(chn)) {
            printf("%s\n", iio_channel_get_id(chn));
            channel_count++;
        }
    }
    if (channel_count == 0) {
        printf("No scan elements found (make sure the driver built with 'CONFIG_IIO_SIMPLE_DUMMY_BUFFER=y')\n");
        shutdown();
    }
    channels = calloc(channel_count, sizeof *channels);
    if (!channels) {
        perror("Channel array allocation failed");
        shutdown();
    }
    for (i = 0; i < channel_count; ++i) {
        struct iio_channel *chn = iio_device_get_channel(dev, i);
        if (iio_channel_is_scan_element(chn))
            channels[i] = chn;
    }
    printf("* Acquiring trigger %s\n", trigger_str);
    trigger = iio_context_find_device(ctx, trigger_str);
    if (!trigger || !iio_device_is_trigger(trigger)) {
        perror("No trigger found (try setting up the iio-trig-hrtimer module)");
        shutdown();
    }
    printf("* Enabling IIO streaming channels for buffered capture\n");
    for (i = 0; i < channel_count; ++i)
        iio_channel_enable(channels[i]);
    printf("* Enabling IIO buffer trigger\n");
    if (iio_device_set_trigger(dev, trigger)) {
        perror("Could not set trigger\n");
        shutdown();
    }
    printf("* Creating non-cyclic IIO buffers with %d samples\n", buffer_length);
    rxbuf = iio_device_create_buffer(dev, buffer_length, false);
    if (!rxbuf) {
        perror("Could not create buffer");
        shutdown();
    }
    printf("* Starting IO streaming (press CTRL+C to cancel)\n");
    bool has_ts = strcmp(iio_channel_get_id(channels[channel_count-1]), "timestamp") == 0;
    int64_t last_ts = 0;
    while (!stop)
    {
        ssize_t nbytes_rx;
        /* we use a char pointer, rather than a void pointer, for p_dat & p_end
         * to ensure the compiler understands the size is a byte, and then we
         * can do math on it.
         */
        char *p_dat, *p_end;
        ptrdiff_t p_inc;
        int64_t now_ts;
        // Refill RX buffer
        nbytes_rx = iio_buffer_refill(rxbuf);
        if (nbytes_rx < 0) {
            printf("Error refilling buf: %d\n", (int)nbytes_rx);
            shutdown();
        }
        p_inc = iio_buffer_step(rxbuf);
        p_end = iio_buffer_end(rxbuf);
        // Print timestamp delta in ms
        if (has_ts)
            for (p_dat = iio_buffer_first(rxbuf, channels[channel_count-1]); p_dat < p_end; p_dat += p_inc) {
                now_ts = (((int64_t *)p_dat)[0]);
                printf("[%04" PRId64 "] ", last_ts > 0 ? (now_ts - last_ts)/1000/1000 : 0);
                last_ts = now_ts;
            }
        // Print each captured sample
        switch (buffer_read_method)
        {
        case BUFFER_POINTER:
            for (i = 0; i < channel_count; ++i) {
                const struct iio_data_format *fmt = iio_channel_get_data_format(channels[i]);
                unsigned int repeat = has_repeat ? fmt->repeat : 1;
                printf("%s ", iio_channel_get_id(channels[i]));
                for (p_dat = iio_buffer_first(rxbuf, channels[i]); p_dat < p_end; p_dat += p_inc) {
                    for (j = 0; j < repeat; ++j) {
                        if (fmt->length/8 == sizeof(int16_t))
                            printf("%" PRIi16 " ", ((int16_t *)p_dat)[j]);
                        else if (fmt->length/8 == sizeof(int64_t))
                            printf("%" PRId64 " ", ((int64_t *)p_dat)[j]);
                    }
                }
            }
            printf("\n");
            break;
        case SAMPLE_CALLBACK: {
            int ret;
            ret = iio_buffer_foreach_sample(rxbuf, sample_cb, NULL);
            if (ret < 0) {
                char buf[256];
                iio_strerror(-ret, buf, sizeof(buf));
                fprintf(stderr, "%s (%d) while processing buffer\n", buf, ret);
            }
            printf("\n");
            break;
        }
        case CHANNEL_READ_RAW:
        case CHANNEL_READ:
            for (i = 0; i < channel_count; ++i) {
                uint8_t *buf;
                size_t sample, bytes;
                const struct iio_data_format *fmt = iio_channel_get_data_format(channels[i]);
                unsigned int repeat = has_repeat ? fmt->repeat : 1;
                size_t sample_size = fmt->length / 8 * repeat;
                buf = malloc(sample_size * buffer_length);
                memset(buf, 0, sizeof(buf));

                if (!buf) {
                    perror("trying to allocate memory for buffer\n");
                    shutdown();
                }
                if (buffer_read_method == CHANNEL_READ_RAW)
                    bytes = iio_channel_read_raw(channels[i], rxbuf, buf, sample_size * buffer_length);
                else
                    bytes = iio_channel_read(channels[i], rxbuf, buf, sample_size * buffer_length);
                printf("Channel Name: %s\n", iio_channel_get_id(channels[i]));
                for (int j = 0; j < sample_size * buffer_length; ++j) {
                    printf("%d ", buf[j]);
                }
                printf("\n\n\n\n");
                free(buf);
            }
            printf("\n");
            break;
        }
        if (count != -1 && --count == 0)
            break;
    }
    shutdown();
    return 0;
}