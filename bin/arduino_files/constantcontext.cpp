#include "constantcontext.h"


ConstantContext::ConstantContext(const char* aId,
        ValveManager* aValves, 
        Communicator* aCommunicator) : 
        Context(aId, aValves, aCommunicator) 
{
    debug("constant context created");
}


ConstantContext::ConstantContext(const char* aId, 
        JsonObject& params,
        ValveManager* aValves, 
        Communicator* aCommunicator) :
        Context(aId, params, aValves, aCommunicator)
{
    debug("constant context created");
}
