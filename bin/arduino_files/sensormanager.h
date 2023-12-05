#ifndef h_sensormanager
#define h_sensormanager

#include <Arduino.h>
#include <ArduinoJson.h>

#include "staticsimplelist.h"
#include "communicator.h"
#include "lickport.h"

class SensorManager
{
public:
    SensorManager(Communicator* aComm = NULL);

    void action(JsonObject& json);
    bool create(JsonObject& params);
    void detect();
    bool stateAt(int pin);
    Sensor* at(int pin);
    void remove(int pin);
    int getSize();

private:
    StaticSimpleList<Sensor, 10, sizeof(RfidReader)> sensors;
    Communicator* myComm;

    void debug(char* msg);
    
    bool memCheck(char* mem);
};

#endif
