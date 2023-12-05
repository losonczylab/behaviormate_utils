#include "context.h"

Context::Context(const char* aId,
                 ValveManager* aValves,
                 Communicator* aCommunicator)
{
    sprintf(id, "%s", aId);
    valveManager = aValves;
    myComm = aCommunicator;
    active = false;
    activate_time = 0;
    timeout = 0;
    frequency = 0;

    initMessages();
}


Context::Context(const char* aId,
                 JsonObject& params,
                 ValveManager* aValves,
                 Communicator* aCommunicator)
{
    sprintf(id, "%s", aId);
    valveManager = aValves;
    myComm = aCommunicator;
    active = false;
    activate_time = 0;
    timeout = 0;
    frequency = 0;

    if (params.containsKey("valves"))
    {
        JsonArray& valveList = params["valves"].asArray();
        JsonArray& durationsList = params["durations"].asArray();
        JsonArray::iterator it = valveList.begin();
        if (valveList != JsonArray::invalid())
        {
            for (int i = 0; it != valveList.end(); ++it, ++i)
            {
                if (durationsList == JsonArray::invalid())
                {
                    addValve(valveList[i].as<int>(),-1);
                }
                else
                {
                    addValve(valveList[i].as<int>(),durationsList[i].as<int>());
                }
            }
        }
        else
        {
            debug("valve list not valid");
        }
    }

    if (params.containsKey("frequency"))
    {
        frequency = params["frequency"].as<int>();
    }

    if (params.containsKey("timeout"))
    {
        timeout = params["timeout"].as<int>();
    }

    initMessages();
}

void Context::initMessages()
{
    JsonObject& start_subjson = this->jsonBuffer.createObject();
    start_subjson["id"] = this->id;
    start_subjson["action"] = "start";
    this->start_json["context"] = start_subjson;

    JsonObject& stop_subjson = this->jsonBuffer.createObject();
    stop_subjson["id"] = this->id;
    stop_subjson["action"] = "stop";
    this->stop_json["context"] = stop_subjson;

    JsonObject& create_subjson = this->jsonBuffer.createObject();
    create_subjson["id"] = this->id;
    create_subjson["action"] = "created";
    JsonObject& create_json = this->jsonBuffer.createObject();
    create_json["context"] = create_subjson;
    myComm->sendMessage(create_json);
}

const char* Context::getId()
{
    return id;
}


void Context::addValve(int pin, int duration)
{
    char* vMem;
    char* dMem;

    vMem=valves.append();
    dMem=durations.append();

    if((!memCheck(vMem)) || (!memCheck(dMem)))
    {
        return;
    }
    *((int*) vMem)=pin;
    *((int*) dMem)=duration;
}


void Context::setActive(bool isActive)
{
    if (isActive)
    {
        myComm->sendMessage(this->start_json);
        activate_time = millis();
    }
    else
    {
        myComm->sendMessage(this->stop_json);
    }
    this->active = isActive;

    _setActive(isActive);
}

// this virtual method is necessary for some subclasses of
// context
void Context::_setActive(bool isActive)
{
    openValves(isActive);
}


void Context::openValves(bool open)
{
    if (open)
    {
        for (int i = 0; i < valves.length(); i++)
        {
            int thisDuration = -1;
            if (i < durations.length())
            {
                thisDuration = *durations.at(i);
            }
            if (this->frequency == 0)
            {
                if (!valveManager->open(*valves.at(i), thisDuration))
                {
                    char buf[256];
                    sprintf(buf, "context %s failed to open valve %d", this->id,
                        *valves.at(i));
                    myComm->sendMessage(buf);
                    setActive(false);

                    return;
                }
            }
            else
            {
                if (!valveManager->open(*valves.at(i), thisDuration, this->frequency))
                {
                    char buf[256];
                    sprintf(buf, "context %s failed to open valve %d", this->id,
                        *valves.at(i));
                    myComm->sendMessage(buf);
                    setActive(false);

                    return;
                }
            }
        }
    }
    else
    {
        for (int i = 0; i < valves.length(); i++)
        {
            if ((durations.length() != valves.length()) || (*durations.at(i) == -1))
            {
                valveManager->close(*valves.at(i));
            }

        }
    }
}

void Context::update()
{
    if (!active)
    {
        return;
    }

    if ((this->timeout > 0) && ((activate_time+timeout) < millis()))
    {
        myComm->sendMessage("timeout");
        setActive(false);
    }

    _update();
}


bool Context::isActive()
{
    return active;
}


void Context::debug(char* msg)
{
    if (this->myComm != NULL)
    {
        this->myComm->debugMessage(msg);
    }
}

void Context::debug(int val)
{
    char buf[30];
    debug(itoa(val, buf, 10));
}

void Context::error(char* msg)
{
    if (this->myComm != NULL)
    {
        this->myComm->errorMessage(msg);
    }
}

bool Context::memCheck(char* mem)
{
    if(mem==nullptr)
    {
        debug("maximum number of valves reached");
        myComm->errorMessage("maximum number of valves reached");
        return false;
    }
    return true;
}
