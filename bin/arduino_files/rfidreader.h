#ifndef __TREADMILL_H__
#define __TREADMILL_H__

#include <Arduino.h>

class RfidReader : public Sensor {
public:
    RfidReader(JsonObject& params, Communicator* communicator);
    bool detect();

private:       
    char prevTag[20];
    char currentTag[20];
    int read_index;
    Communicator* communicator;
    unsigned long lastUpdate;
    int report_pin;
    bool reading;
};

#endif
