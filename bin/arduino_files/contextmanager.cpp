#include "contextmanager.h"

ContextManager::ContextManager(
    ValveManager* aValveManager,
    SensorManager* aSensorManager,
    Communicator* aComm)
{
    valveManager = aValveManager;
    sensorManager = aSensorManager;
    myComm = aComm;
}


void ContextManager::action(JsonObject& json)
{
    if (!json.containsKey("action"))
    {
        return;
    }

    const char* action = json["action"].asString();
    if (strcmp(action,"create") == 0)
    {
        if (!create(json))
        {
            debug("failed to create context");
        }
    }
    else if (strcmp(action,"start") == 0)
    {
        if (!startContext(json["id"].asString()))
        {
            char buf[64];
            for (int i = 0; i < myContexts.length(); i++)
            {
                sprintf(buf, "%s - %s, %i", json["id"].as<char*>(), myContexts.at(i)->getId(),
                    strcmp(myContexts.at(i)->getId(),json["id"].asString()));
                debug(buf);
            }
            debug("failed to start context");
        }
    }
    else if (strcmp(action,"stop") == 0)
    {
        if (!stopContext(json["id"].asString()))
        {
            char buf[64];
            for (int i = 0; i < myContexts.length(); i++)
            {
                sprintf(buf, "%s - %s, %i", json["id"].as<char*>(), myContexts.at(i)->getId(),
                    strcmp(myContexts.at(i)->getId(),json["id"].asString()));
                debug(buf);
            }
            debug("failed to stop context");
        }
    }
    else if (strcmp(action, "activate"))
    {
        get(json["id"].asString())->setActive(true);
    }
    else
    {
        debug("action not understood");
    }
}


bool ContextManager::create(JsonObject& params)
{
    if (!params.containsKey("id"))
    {
        debug("failed to find id field");
        return false;
    }

    const char* id = params["id"].as<const char*>();
    char buf[64];
    sprintf(buf, "id: %s", id);
    debug(buf);
    int index = getIndex(id);
    if (index != -1)
    {
        sprintf(buf, "redefining context: %s", id);
        debug(buf);

//         Context* rm_context = myContexts.at(index);
        myContexts.remove(index);
//         delete rm_context;
    }

    char* cMem=myContexts.append();
    if(!memCheck(cMem))
    {
        return false;
    }
    if (params.containsKey("type"))
    {
        debug("understanding type");
        const char* type = params["type"].as<const char*>();
        
        if (strcmp(params["type"].asString(), "operant") == 0)
        {
            Context* nCtxt=new (cMem) OperantContext(id,params,valveManager,sensorManager,myComm);
//             myContexts.append(
//                 new OperantContext(id,params,valveManager,sensorManager,myComm));
        }
        else if (strcmp(params["type"].asString(), "pulsed") == 0)
        {
            Context* nCtxt=new (cMem) PulsedContext(id,params,valveManager,myComm);
//             myContexts.append(new PulsedContext(id,params,valveManager,myComm));
        }
        else
        {
            Context* nCtxt=new (cMem) Context(id,params,valveManager,myComm);
//             myContexts.append(new Context(id,params,valveManager,myComm));
        }
    }
    else
    {
        Context* nCtxt=new (cMem) Context(id,params,valveManager,myComm);
//         myContexts.append(new Context(id,params,valveManager,myComm));
    }

    return true;
}


bool ContextManager::create(char* id, int valves[], int durations[], int operantRate, int sensor)
{
    Context* newContext = NULL;
    char* cMem=myContexts.append();
    if(!memCheck(cMem))
    {
        return false;
    }
    if (operantRate > 0)
    {
        newContext = new (cMem) OperantContext(
            id, operantRate, sensor, valveManager, sensorManager, myComm);
    }
    else
    {
        newContext = new (cMem) ConstantContext(id, valveManager, myComm);
    }

    int len = sizeof(valves)/sizeof(valves[0]);
    for (int i = 0; i < len; i++)
    {
        newContext->addValve(valves[i], durations[i]);
    }
//     myContexts.append(newContext);
}


bool ContextManager::startContext(const char* id)
{
    debug("starting context\n");
    Context* thisContext = get(id);
    if (thisContext != NULL)
    {
        thisContext->setActive();
        return true;
    }

    return false;
}


bool ContextManager::stopContext(const char* id)
{
    Context* thisContext = get(id);
    if (thisContext != NULL)
    {
        thisContext->setActive(false);
        return true;
    }

    return false;
}


Context* ContextManager::get(const char* id)
{
    for (int i = 0; i < myContexts.length(); i++)
    {
        if (strcmp(myContexts.at(i)->getId(),id) == 0)
        {
            return myContexts.at(i);
        }
    }

    return NULL;
}

int ContextManager::getIndex(const char* id)
{
    for (int i = 0; i < myContexts.length(); i++)
    {
        if (strcmp(myContexts.at(i)->getId(),id) == 0)
        {
            return i;
        }
    }

    return -1;
}


void ContextManager::update()
{
    for (int i = 0; i < myContexts.length(); i++)
    {
        myContexts.at(i)->update();
    }
}


void ContextManager::debug(char* msg)
{
    if (myComm != NULL)
    {
        myComm->debugMessage(msg);
    }
}


void ContextManager::debug(int val)
{
    if (myComm != NULL)
    {
        char buf[30];
        debug(itoa(val,buf,10));
    }
}

bool ContextManager::memCheck(char* mem)
{
    if(mem==nullptr)
    {
        debug("maximum number of contexts reached");
        myComm->errorMessage("maximum number of contexts reached");
        return false;
    }
    return true;
}
