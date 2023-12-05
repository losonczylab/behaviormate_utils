#ifndef h_cap1188_mapping
#define h_cap1188_mapping

#include <Arduino.h>
#include <ArduinoJson.h>

#include "communicator.h"

class Cap1188Mapping : Sensor
{
public:
    Cap1188Mapping(JsonObject& params,
           Cap1188* aCap1188,
           Communicator* aCommunicator=NULL);
    bool detect();

protected:
    int sensor;
    Cap1188* cap1188;
    
    void initMessages();
};

#endif
