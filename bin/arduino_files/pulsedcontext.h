#ifndef h_pulsedcontext
#define h_pulsedcontext

#include <Arduino.h>
#include <ArduinoJson.h>

#include "simplelist.h"
#include "valve.h"
#include "communicator.h"
#include "valvemanager.h"
#include "context.h"

class PulsedContext : public Context {
public:
    PulsedContext(const char* aId,
            int interval,
            ValveManager* aValves,
            Communicator* aCommunicator=NULL);
    PulsedContext(const char* aId,
            JsonObject& params,
            ValveManager* aValves,
            Communicator* aCommunicator=NULL);

protected:
    void _update();
    void _setActive(bool isActive);

private:
    int interval;
    unsigned long next_start;
};

#endif
