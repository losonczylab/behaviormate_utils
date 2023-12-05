#include "operantcontext.h"


OperantContext::OperantContext(const char* aId,
        int aOperantRate,
        int aSensor,
        ValveManager* aValves,
        SensorManager* aSensorManager,
        bool aInitialOpen,
        Communicator* aCommunicator) :
        Context(aId, aValves, aCommunicator)
{
    count = 0;
    sensor = aSensor;
    sensorManager = aSensorManager;
    operantRate = aOperantRate;
    initialOpen = aInitialOpen;
}


OperantContext::OperantContext(const char* aId,
        JsonObject& params,
        ValveManager* aValves,
        SensorManager* aSensorManager,
        Communicator* aCommunicator) :
        Context(aId, params, aValves, aCommunicator)
{
    sensorManager = aSensorManager;
    count = 0;
    if (params.containsKey("operant_rate"))
    {
        operantRate = params["operant_rate"].as<int>();
    }
    else
    {
        operantRate = 1;
    }

    if (params.containsKey("initial_open")
        && (params["initial_open"].as<int>() == 1))
    {
        initialOpen = true;
    }
    else
    {
        initialOpen = false;
    }

    if (params.containsKey("sensor"))
    {
        sensor = params["sensor"].as<int>();
    }
    else
    {
        sensor = 0;
    }

    if (params.containsKey("enable_first"))
    {
        this->enable_first = params["enable_first"].as<bool>();
    }

    if (params.containsKey("max_count"))
    {
        this->max_count = params["max_count"].as<int>();
    }
    else
    {
        this->max_count = -1;
    }
    this->open_count = 0;

    char buf[128];
    sprintf(buf, "operate context created, operate_rate: %i, sensor_pin: %i",
            operantRate, sensor);
    debug(buf);
}


void OperantContext::_update()
{
    if ((this->max_count == -1) || (this->open_count < this->max_count))
    {
        if (sensorManager->stateAt(sensor))
        {
            count++;
            if (count == operantRate)
            {
                openValves();
                count = 0;
                this->open_count++;
            }
        }
    }
}

void OperantContext::_setActive(bool isActive)
{
    if (this->enable_first)
    {
        this->count = operantRate-1;
    }
    else
    {
        count = 0;
    }

    this->open_count = 0;
    if (active && initialOpen) {
        openValves();
        this->open_count++;
    }
}
