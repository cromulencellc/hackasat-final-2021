#include <Arduino.h>

#include <string>
// #include "TimeLib.h"
#include "pdb_pins.h"
#include <pdb_app.h>
#include <pdb_logger.hpp>

uint32_t previousMillis;
uint16_t loopInterval = 1000;
PdbApp pdb;



//Instantiate with reference to global set
// IntervalTimer pdbUpdateTimer;

// JRL - PDB FSW requirements
// 1. Control 5V output enable via command - Done
// 2. Set 5V output initial state at startup - Done
// 3. Provide Telemetry data to the C&DH via MQTT (CCSDS formatted payload) - Done over UDP, not MQTT
// 4. Configure charge controller via batt scl/sda - Done
// 5. Read temperature sensor via TS scl/sda
// 6. USB-PD controller configuration?
// 7. Read analog voltages/currents


time_t getTeensy3Time()
{
    return Teensy3Clock.get();
}

/*  code to process time sync messages from the Serial2 port   */
#define TIME_HEADER  "T"   // Header tag for Serial2 time sync message

unsigned long processSyncMessage() {
    unsigned long pctime = 0L;
    const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013

    if(Serial2.find(TIME_HEADER)) {
         pctime = Serial2.parseInt();
         return pctime;
         if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
             pctime = 0L; // return 0 to indicate that the time is not valid
         }
    }
    return pctime;
}

void setup() { 
    Serial2.begin(115200);
    LOG_INFO("PDB Setup Started");
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    // setSyncProvider(getTeensy3Time);
    Wire.begin();
    pdb.begin();
    delay(500);

    // Serial2.printf("Voltage Regulator Status. VR1: %d, VR2: %d\n", digitalRead(VR_1_PWR_GOOD), digitalRead(VR_2_PWR_GOOD));
    // Serial2.printf("Voltage Channel: %d, Current Channel: %d\n", analogRead(ANALOG_CH_VOLT_IN), analogRead(ANALOG_CH_CUR_IN));
    LOG_INFO("PDB Setup Complete");
}



void loop() {
    // Serial2.printf("Voltage Regulator Status. VR1: %d, VR2: %d\n", digitalRead(VR_1_PWR_GOOD), digitalRead(VR_2_PWR_GOOD));
    // Serial2.printf("Voltage Channel: %d, Current Channel: %d\n", analogRead(ANALOG_CH_VOLT_IN), analogRead(ANALOG_CH_CUR_IN));

    uint32_t currentMillis = millis();

    if( currentMillis - previousMillis > loopInterval ){
        previousMillis = currentMillis;

        pdb.processCommands();
        pdb.sendTelemetry();
        SERIAL_OUTPUT.print(".");
        // get_usbPdData();
    }
        // digitalWrite(LED1, LOW);
        // delay(50);
                // time_t t = processSyncMessage();
        // if (t != 0) {
        //     Teensy3Clock.set(t); // set the RTC
        //     setTime(t);
        // }

}