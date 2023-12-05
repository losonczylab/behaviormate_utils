#include "valvemanager.h"

ValveManager::ValveManager(Communicator* aComm)
{
    myComm = aComm;
    debug("valve manager created");
}


ValveManager::~ValveManager()
{
    debug("destroying valve manager");
}


void ValveManager::action(JsonObject& json)
{
    if (!json.containsKey("action"))
    {
        return;
    }

    const char* action = json["action"].asString();
    int pin = -1;

    if (json.containsKey("pin"))
    {
        pin = json["pin"].as<int>();
    }
    else
    {
        debug("no pin recieved for valve");
        return;
    }

    if (strcmp(action, "create") == 0)
    {
        if (atPin(pin) != NULL)
        {
            remove(pin);
        }

        if (json.containsKey("frequency"))
        {
            char* valveMem=myValves.append();
            if(!memCheck(valveMem))
            {
                return;
            }
            Tone* newV=new (valveMem) Tone(pin, json["frequency"].as<int>(), myComm);
        }
        else if (json.containsKey("type"))
        {
            if (strcmp(json["type"].asString(), "tone") == 0)
            {
                char* valveMem=myValves.append();
                if(!memCheck(valveMem))
                {
                    return;
                }
                Tone* newV=new (valveMem) Tone(pin, myComm);
            }
        }
        else if (json.containsKey("inverted") && json["inverted"])
        {
            char* valveMem=myValves.append();
            if(!memCheck(valveMem))
            {
                return;
            }
            InvertedValve* newV=new (valveMem) InvertedValve(pin, myComm);
        }
        else
        {
            createValve(pin);
        }

        return;
    }

    Valve* thisValve = atPin(pin);
    if (thisValve == NULL)
    {
        if (json.containsKey("frequency"))
        {
            char* valveMem=myValves.append();
            if(!memCheck(valveMem))
            {
                return;
            }
            Tone* newV=new (valveMem) Tone(pin, json["frequency"].as<int>(), myComm);
        }
        else
        {
            createValve(pin);
        }
        thisValve = atPin(pin);
    }

    if (strcmp(action,"open") == 0)
    {
        int duration = -1;
        if (json.containsKey("duration"))
        {
            duration = json["duration"].as<int>();
        }

        if (json.containsKey("frequency"))
        {
            ((Tone*) thisValve)->open(duration, json["frequency"].as<int>());
        }
        else
        {
            thisValve->open(duration);
        }
    }
    else if (strcmp(action,"close") == 0)
    {
        thisValve->close();
    }
}


void ValveManager::createValve(int pin)
{
    char* valveMem=myValves.append();
    if(!memCheck(valveMem))
    {
        return;
    }
    Valve* newV=new (valveMem) Valve(pin, myComm);
}


void ValveManager::update()
{
    for (int i = 0; i < myValves.length(); i++)
    {
        myValves.at(i)->check();
    }
}


bool ValveManager::open(int pin, int duration)
{
    Valve* valve = atPin(pin);
    if (valve != NULL)
    {
        valve->open(duration);
        return true;
    }

    return false;
}


bool ValveManager::open(int pin, int duration, int frequency)
{
    Valve* valve = atPin(pin);
    if (valve != NULL)
    {
        ((Tone*) valve)->open(duration, frequency);
    }
}


bool ValveManager::close(int pin)
{
    Valve* valve = atPin(pin);
    if (valve != NULL)
    {
        valve->close();
        return true;
    }

    return false;
}


void ValveManager::openAll()
{
    debug("opening all");
    for (int i = 0; i < myValves.length(); i++)
    {
        myValves.at(i)->open();
    }
}


void ValveManager::closeAll()
{
    debug("closing all");
    for (int i = 0; i < myValves.length(); i++)
    {
        myValves.at(i)->close();
    }
}


Valve* ValveManager::atPin(int pin)
{
    for (int i = 0; i < myValves.length(); i++)
    {
        if (myValves.at(i)->getPin() == pin)
        {
            return myValves.at(i);
        }
    }
    debug("solenoid not found");

    return NULL;
}


bool ValveManager::remove(int pin)
{
    for (int i = 0; i < myValves.length(); i++)
    {
        if (myValves.at(i)->getPin() == pin)
        {
            myValves.remove(i);
            return true;
        }
    }
    debug("remove: solenoid not found");

    return false;
}


void ValveManager::debug(char* msg)
{
    if (myComm != NULL)
    {
        myComm->debugMessage(msg);
    }
}

void ValveManager::debug(int val)
{
    if (myComm != NULL)
    {
        char buf[30];
        debug(itoa(val,buf,10));
    }
}

bool ValveManager::memCheck(char* mem)
{
    if(mem==nullptr)
    {
        debug("maximum number of valves reached");
        myComm->errorMessage("maximum number of valves reached");
        return false;
    }
    return true;
}
