#include "pulsedcontext.h"

PulsedContext::PulsedContext(const char* aId,
        int aInterval,
        ValveManager* aValves,
        Communicator* aCommunicator) :
        Context(aId, aValves, aCommunicator)
{
    this->interval = aInterval;
    this->next_start = 0;
}


PulsedContext::PulsedContext(const char* aId,
        JsonObject& params,
        ValveManager* aValves,
        Communicator* aCommunicator) :
        Context(aId, params, aValves, aCommunicator)
{
    if (params.containsKey("interval"))
    {
        this->interval = params["interval"].as<int>();
    }
    else
    {
        this->error("pulsed context requires \"interval\" to be set");
        return;
    }

    this->next_start = 0;

    char buf[128];
    sprintf(buf, "pulsed context created, interval: %i", this->interval);
    myComm->sendMessage(buf);
}


void PulsedContext::_setActive(bool isActive)
{
    this->next_start = millis() + this->interval;
    openValves(isActive);
}

void PulsedContext::_update()
{
    unsigned long time = millis();
    if (time > this->next_start)
    {
        openValves(true);
        this->next_start = time + this->interval;
    }
}

