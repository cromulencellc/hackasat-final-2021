
#ifndef _NX3L4051_MUX_H_
#define _NX3L4051_MUX_H_

#include "Arduino.h"

class NX3L4051Mux {

    #define MUX_CHANNEL_MAX 7

    public:
        void begin(uint8_t PIN_EN, uint8_t PIN_S1, uint8_t PIN_S2, uint8_t PIN_S3);
        void enableChannel(uint8_t channelNumber);
        void disable(void);
        void enable(void);
        bool isEnabled(void);

    private:
        bool enabled_;
        uint8_t pin_en_;
        uint8_t pin_s3_;
        uint8_t pin_s2_;
        uint8_t pin_s1_;
};

#endif