#include "NX3L4051.h"
// #include "pdb_logger.hpp"
#define MODE_DELAY_NSEC 500

void NX3L4051Mux::begin(uint8_t PIN_EN, uint8_t PIN_S1, uint8_t PIN_S2, uint8_t PIN_S3) {

    pin_en_ = PIN_EN;
    pin_s3_ = PIN_S3;
    pin_s2_ = PIN_S2;
    pin_s1_ = PIN_S1;
    pinMode(pin_en_, OUTPUT);
    pinMode(pin_s3_, OUTPUT);
    pinMode(pin_s2_, OUTPUT);
    pinMode(pin_s1_, OUTPUT);
    disable();

}

void NX3L4051Mux::disable() {
    delayNanoseconds(MODE_DELAY_NSEC);
    digitalWrite(pin_en_, HIGH);
    enabled_ = false;
    delayNanoseconds(MODE_DELAY_NSEC);
    // LOG_DEBUG("MUX Disabled");
}

void NX3L4051Mux::enable() {
    delayNanoseconds(MODE_DELAY_NSEC);
    digitalWrite(pin_en_, LOW);
    enabled_ = true;
    delayNanoseconds(MODE_DELAY_NSEC);
    // LOG_DEBUG("MUX Enabled");
}

void NX3L4051Mux::enableChannel(uint8_t channelNumber) {
    channelNumber = channelNumber & 0x7;
    // LOG_DEBUG("MUX ENABLE CH %d", channelNumber);
    disable();
    if (channelNumber <= MUX_CHANNEL_MAX) {
        digitalWrite(pin_s1_, (channelNumber) & 0x1);
        digitalWrite(pin_s2_, (channelNumber >> 0x1) & 0x1);
        digitalWrite(pin_s3_, (channelNumber >> 0x2) & 0x1);
    }
    enable();
    // LOG_DEBUG("MUX ENABLE Complete");
}

bool NX3L4051Mux::isEnabled(void) {
    return enabled_;
}