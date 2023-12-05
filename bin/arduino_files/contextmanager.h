#ifndef n_contextmanager
#define h_contextmanager

#include <Arduino.h>
#include <ArduinoJson.h>

#include "context.h"
#include "staticsimplelist.h"
#include "communicator.h"
#include "valvemanager.h"
#include "operantcontext.h"
#include "pulsedcontext.h"
#include "sensormanager.h"
#include "constantcontext.h"

class ContextManager
{
public:
    ContextManager(ValveManager* aValveManager,
        SensorManager* aSensorManager,
        Communicator* aComm = NULL);

    void action(JsonObject& json);
    bool startContext(const char* id);
    bool stopContext(const char* id);
    bool create(JsonObject& params);
    bool create(char* id, int valves[], int durations[], int operantRate=-1, int sensor=0);
    void update();
    int getIndex(const char* id);

private:
    StaticSimpleList<Context, 15, sizeof(OperantContext)> myContexts;
    ValveManager* valveManager;
    SensorManager* sensorManager;
    Communicator* myComm;

    Context* get(const char* id);
    void debug(char* msg);
    void debug(int val);
    bool memCheck(char* mem);
};

#endif
