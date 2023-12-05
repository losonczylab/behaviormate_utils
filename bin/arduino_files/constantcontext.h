#ifndef h_constantcontext
#define h_constantcontext

#include <Arduino.h>
#include <ArduinoJson.h>

#include "simplelist.h"
#include "valve.h"
#include "communicator.h"
#include "valvemanager.h"
#include "context.h"

class ConstantContext : public Context {
public:
    ConstantContext(const char* aId, 
            ValveManager* aValves, 
            Communicator* aCommunicator=NULL);
    ConstantContext(const char* aId, 
            JsonObject& params,
            ValveManager* aValves, 
            Communicator* aCommunicator=NULL);

};

#endif
