#ifndef h_valve
#define h_valve

#include <Arduino.h>
#include <ArduinoJson.h>

#include "communicator.h"

#define MAX_WAIT 10

class Valve
{
public:
    Valve();

    Valve(int pin, Communicator* aCommunicator=NULL);

    // check the solenoid to see if it needs to be closed
    // based on the timeout
    void check();

    // check if the solenoid is currently opened
    bool isOpen();

    // open the solenoid for a specified amound of time
    // time < 0 means the vavle will remain opened
    virtual void open(int time=-1);

    // close the valve reqaurdless of the durations specified
    virtual void close();

    // returns the pin number this solenoid is connected to
    int getPin();

protected:
    int pinNumber;
    StaticJsonBuffer<512> jsonBuffer;
    char pinNumber_str[2];
    JsonObject& open_json = jsonBuffer.createObject();
    JsonObject& close_json = jsonBuffer.createObject();
    long openedAt;
    int duration;
    Communicator* communicator;

    virtual void initMessages();

    void debug(char* msg);
    void debug(int val);
};

#endif
