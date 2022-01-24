#include <Arduino.h>
#include <pdb_output.h>
#include <pdb_logger.hpp>

void PdbOutputChannel::begin(uint8_t pin, ChannelState initialState, uint8_t channelId, bool protectedChannel) {
    protected_ = protectedChannel;
    pin_ = pin;
    state_ = initialState;
    channelId_ = channelId;
    displayId_ = channelId + 1;
    pinMode(pin_, OUTPUT);
    setState(state_);
}

void PdbOutputChannel::on(void) {
    setState(ChannelState::ON);
}

void PdbOutputChannel::off(void) {
    setState(ChannelState::OFF);
}

uint8_t PdbOutputChannel::getGpioState(void) {
    return digitalRead(pin_);
}

uint8_t PdbOutputChannel::getState(void) {
    return static_cast<uint8_t>(state_);
}

void PdbOutputChannel::setState(ChannelState state) {
    state_ = state;
    if(state == ChannelState::ON) {
        LOG_WARN("Channel %d -> ON", displayId_);
        digitalWrite(pin_, HIGH);
    }
    if(state == ChannelState::OFF) {
        LOG_WARN("Channel %d -> OFF", displayId_);
        digitalWrite(pin_, LOW);
        if (protected_) {
            LOG_WARN("Protected Channel, Return to ON after 100 msec");
            delay(100);
            this->on();
        }
    }
}