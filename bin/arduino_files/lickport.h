#ifndef h_lickport
#define h_lickport

#include <Arduino.h>
#include <Wire.h>
#include <ArduinoJson.h>

#include "communicator.h"
#include "mprvalues.h"
#include "sensor.h"

#define SENSOR_ADDRESS  0x5A


class LickPortClass : public Sensor
{
public:
    LickPortClass(JsonObject& params, bool initialize, Communicator* aCommunicator);
    bool detect();

private:
    bool checkInterrupt();
    void mpr121_setup();
    void set_register(int address, unsigned char r, unsigned char v);
    void initMessages();

    int report_pin;
    int start_thresh;
    int stop_thresh;
    int sensor;
    // bool prev_state;
    bool inLick;
};

#endif
