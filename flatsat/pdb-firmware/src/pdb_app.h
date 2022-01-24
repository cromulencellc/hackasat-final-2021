#ifndef _pdb_app_h
#define _pdb_app_h


#include "Arduino.h"
#include <array>
#include <pdb_comms.h>
#include <pdb_output.h>
#include <Wire.h>
#include <SparkFun_STUSB4500.h>
#include <Lorro_BQ25703A.h>
#include <NX3L4051.h>
#include <ADC.h>
// #include <ADC_util.h>
extern "C" {
    #include "app_cfg.h"
    #include "pdh_msg.h"
    #include "pdb_pins.h"
}


class PdbApp {
    
    #define MAX_MUX_CH_TO_READ 6

    struct UsbPdOutputData {
        float Voltage;
        float Current;
        float LowerVoltageLimit;
        float UpperVoltageLimit;
    };

    struct UsbPdData {
        uint8_t externalPowerCapable;
        uint8_t usbCommCapable;
        uint8_t configOk;
        uint8_t gpioCtrl;
        uint8_t pwrOnlyAbove5V;
        uint8_t reqSrcCurrent;
        float flexCurrent;
        struct UsbPdOutputData usbpd1;
        struct UsbPdOutputData usbpd2;
        struct UsbPdOutputData usbpd3;
    };

    public:
        PdbApp();
        void begin(void);
        int32_t processCommands();
        bool sendTelemetry();
        bool outputEnable(uint8_t channel);
        bool outputDisable(uint8_t channel);
        int32_t updateAnalogTelemetry();
        int32_t updateChargeController();

        // HouseKeeping Status
        uint32_t validCmdCnt;
        uint32_t invalidCmdCnt;
        uint32_t ctrlExeCnt;

        // Regulator States
        uint8_t voltageRegulatorPowerGood1;
        uint8_t voltageRegulatorPowerGood2;

        // Output Switch States
        std::array <PdbOutputChannel,OUTPUT_CHANNEL_COUNT>outputChannels;
        

        // Debug LEDs
        std::array <uint8_t,4>ledPins;
        std::array <uint8_t,4>ledStates;

        // Charger Status
        uint8_t    chargeEnabled;
        uint16_t   maxChargeVoltage;
        uint16_t   maxChargeCurrent;
        uint16_t   systemPowerUtilization;
        uint16_t   vBUS;
        uint16_t   vBAT;
        uint16_t   vSYS;
        uint16_t   actualChargeCurrent;
        uint16_t   actualDischargeCurrent;

        // USB PD Status
        UsbPdData usbPdData;

        // Analog Sensor Values
        std::array <uint16_t,7>channelAnalogVoltageRaw;
        std::array <uint16_t,7> channelAnalogCurrentRaw;
        std::array <uint16_t,7> systemAnalogVoltageRaw;
        std::array <uint16_t,7>channelAnalogVoltage;
        std::array <uint16_t,7> channelAnalogCurrent;
        std::array <uint16_t,7> systemAnalogVoltage;


    private:
        PDB_HkPkt pdbHkPkt_;
        PDB_ChargerPkt pdbChargerPkt_;
        PdbComms pdb_comms_;
        ADC *adc_; 
        Lorro_BQ25703A BQ25703A;
        Lorro_BQ25703A::Regt BQ25703Areg;
        STUSB4500 usb_pd_;
        NX3L4051Mux mux_;
        void init_ADC_(void);
        void init_BQ25703A_(void);
        void init_USBPD_(void);
        bool noopCmd_(uint16_t UserDataLen, const CFE_SB_MsgPtr_t MsgPtr);
        bool resetAppCmd_(uint16_t UserDataLen, const CFE_SB_MsgPtr_t MsgPtr);
        bool setChannelOutputStateCmd_(uint16_t UserDataLen, const CFE_SB_MsgPtr_t MsgPtr);
        int  sendHkTelemetry_(void);
        int  sendChargerTelemetry_(void);
        bool readPdbStates_(void);
        bool readChargerStatus_(void);
        void readUsbPdStatus_(void);
        bool readAnalogTelemetry_(void);
        
};

#endif