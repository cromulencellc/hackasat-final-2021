#include <time.h>
#include "XadcDriver.h"

static xadc_device_t device;
static int keepRunning = 1;

void xadc_trial_runner(unsigned int channelCount, char **channels, unsigned int trials, bool channelReconfigure);
double run(unsigned int numChannels, char **inputChannels, unsigned int samples, bool reconfigure);
int nsleep(long miliseconds);

void intHandler(int value) {
    keepRunning = 0;
}


int main(int argc, char* argv[]) {
     
    signal(SIGINT, intHandler);

    int i;
    unsigned int channelCount = 8;

    // unsigned int channelCount = 2;
    const char **channelNames[8] = {
        "voltage14", 
        "voltage12",
        "voltage9",
        "voltage10",
        "voltage11",
        "voltage8",
        "voltage13",
        "temp0"};
    // unsigned int channelsIds[2] = {11, 10};
    
    // unsigned int channelsIds[1] = {9};
    // unsigned int channelsIds1[1] = {10};
    // unsigned int channelsIds2[1] = {11};



    xadc_constructor(&device);
    if(!xadc_init("iio:device1", "trigger1", XADC_SAMPLE_RATE, false)) {
        ADCS_IO_LOG_ERROR("Init error\n");
        xadc_shutdown();
        exit(1);
    }
    // run(channelCount, channelsIds, 5);
    xadc_trial_runner(channelCount, channelNames, 100, false);
    xadc_shutdown();

    exit(0);

}


double run(unsigned int numChannels, char **inputChannels, unsigned int samples, bool reconfigure) {
    clock_t start_time = clock();
    if (reconfigure || !device.active) {
        xadc_setup_channels(numChannels, inputChannels);
    }
    
    ssize_t xadc_nbytes;
    int counter = 0;
    while(keepRunning) {
        if (device.active) {
            xadc_update();
        } else {
            break;
        }
        counter++;
        if (counter > samples) {
            break;
        }
    }
    if (reconfigure) {
        xadc_stop_channels();
    }
    clock_t stop_time = clock();
    double time_taken = ((double)stop_time-start_time)/CLOCKS_PER_SEC; // in seconds
    // printf("Channel sample took %f sec\n", time_taken);
    return time_taken;
}

void xadc_trial_runner(unsigned int channelCount, char **channels, unsigned int trials, bool channelReconfigure) {
    ADCS_IO_LOG_INFO("Running time trial for %d channels with %d trials\n", channelCount, trials);
    FILE *file = fopen("xadc_trial_runner.csv", "w");
    double results[trials];
    int j;
    memset(results, 0, sizeof(results));
    for (int i=0; i<trials; i++) {
        results[i] = run(channelCount, channels, 1, channelReconfigure);
        for(j=0; j<channelCount; j++) {
            if (j==channelCount-1) {
                fprintf(file, "%f\n", device.channels[j].converted);
            } else {
                fprintf(file, "%f,", device.channels[j].converted);
            }
        }
        nsleep(10);
    }
    fclose(file);
    double mean = array_mean(results, trials);
    double max = array_max(results, trials);
    double min = array_min(results, trials);
    double std = array_std(results, trials);
    ADCS_IO_LOG_INFO("Result Stats (seconds):\n");
    ADCS_IO_LOG_INFO("Mean: %f, Std: %f, Min: %f, Max: %f\n", mean, std, min, max);
}

