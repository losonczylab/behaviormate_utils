#ifndef h_operantcontext
#define h_operantcontext

#include <Arduino.h>
#include <ArduinoJson.h>

#include "simplelist.h"
#include "valve.h"
#include "communicator.h"
#include "valvemanager.h"
#include "context.h"

class OperantContext : public Context {
public:
    OperantContext(const char* aId,
            int aOperantRate,
            int aSensor,
            ValveManager* aValves,
            SensorManager* sensorManager,
            bool aInitialOpen=false,
            Communicator* aCommunicator=NULL);
    OperantContext(const char* aId,
            JsonObject& params,
            ValveManager* aValves,
            SensorManager* sensorManager,
            Communicator* aCommunicator=NULL);

    void _update();

    void _setActive(bool isActive=true);

private:
    int count;
    int operantRate;
    int open_count;
    int max_count;
    int sensor;
    bool initialOpen;
    bool enable_first;
    SensorManager* sensorManager;
};

#endif
