#ifndef _PDB_OUTPUT_H_
#define _PDB_OUTPUT_H_

#include <stdint.h>

enum class ChannelState : uint8_t {
    OFF = 0,
    ON = 1
};


class PdbOutputChannel {
    
    public:
        void begin(uint8_t pin, ChannelState initialState, uint8_t channelId, bool protectedChannel);
        uint8_t getGpioState(void);
        uint8_t getState(void);
        void setState(ChannelState state);
        void on(void);
        void off(void);

    private:
        bool protected_;
        uint8_t channelId_;
        uint8_t displayId_;
        ChannelState state_;
        uint8_t pin_;

};


#endif