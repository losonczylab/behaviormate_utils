#ifndef h_context
#define h_context

#include <Arduino.h>
#include <ArduinoJson.h>

#include "staticsimplelist.h"
#include "valve.h"
#include "communicator.h"
#include "valvemanager.h"

class Context{
public:
    Context(const char* aId,
            ValveManager* aValves,
            Communicator* aCommunicator=NULL);
    Context(const char* aId,
            JsonObject& params,
            ValveManager* aValves,
            Communicator* aCommunicator=NULL);

    const char* getId();
    void addValve(int pin,int duration=-1);
    void setActive(bool isActive=true);
    virtual void _setActive(bool isActive=true);
    bool isActive();
    void update();
    virtual void _update() {}

protected:
    virtual void openValves(bool open=true);
    void debug(char* msg);
    void debug(int val);
    virtual void initMessages();
    ValveManager* valveManager;
    StaticSimpleList<int, 20, sizeof(int)> valves;
    StaticSimpleList<int, 20, sizeof(int)> durations;
    unsigned long activate_time;
    unsigned long timeout;
    int frequency;
    void error(char* msg);
    bool memCheck(char* mem);


    Communicator* myComm;
    char id[64];
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject& start_json = jsonBuffer.createObject();
    JsonObject& stop_json = jsonBuffer.createObject();

    bool active;
};

#endif
