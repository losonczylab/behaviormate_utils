#ifndef h_sensor
#define h_sensor

#include <Arduino.h>
#include <ArduinoJson.h>

#include "communicator.h"

class Sensor
{
public:
    Sensor();
    Sensor(int pin,
           Communicator* aCommunicator=NULL);
    Sensor(JsonObject& params,
           Communicator* aCommunicator=NULL);

    int getPin();
    virtual bool detect();
    bool getState();

protected:
    void debug(char* msg);
    void error(char* msg);
    void virtual initMessages();

    StaticJsonBuffer<1024> jsonBuffer;
    int pin;
    Communicator* communicator;
    JsonObject& start_json = jsonBuffer.createObject();
    JsonObject& stop_json = jsonBuffer.createObject();
    bool state;
    bool status;
};

#endif
