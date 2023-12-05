#include "sensormanager.h"

SensorManager::SensorManager(Communicator* aComm)
{
    myComm = aComm;
    debug("sensor manager created");
}


void SensorManager::action(JsonObject& json)
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
            debug("failed to setup sensor");
        }
    }
    else if (strcmp(action, "clear") == 0)
    {
        while (this->getSize() > 0)
        {
            sensors.remove(0);
        }
    }
}

bool SensorManager::create(JsonObject& params)
{
    int pin = params["pin"].as<int>();

    if (at(pin) != NULL)
    {
        remove(pin);
    }

    if (params.containsKey("type"))
    {
        const char* type = params["type"].asString();
        if (strcmp(type, "lickport") == 0)
        {
            debug("creating new lick port sensor");
            char *senMem=sensors.append();
            if(!memCheck(senMem))
            {
                return false;
            }
            LickPortClass* newS=new (senMem) LickPortClass(params, at(pin) == NULL, myComm);
            return true;
        }

        if (strcmp(type, "rfid_reader") == 0)
        {
            debug("creating new rfid reader");
            char *senMem=sensors.append();
            if(!memCheck(senMem))
            {
                return false;
            }
            RfidReader* newS=new (senMem) RfidReader(params, myComm);            
//             sensors.append(new RfidReader(params, myComm));
            return true;
        }

        if (strcmp(type, "piezoport") == 0)
        {
            debug("creating new lick port piezo sensor");
            char *senMem=sensors.append();
            if(!memCheck(senMem))
            {
                return false;
            }
            LickPortPiezo* newS=new (senMem) LickPortPiezo(params, myComm);            
//             sensors.append(new LickPortPiezo(params, myComm));
            return true;
        }

        if (strcmp(type, "irport") == 0)
        {
            debug("creating new lick port piezo sensor");
            char *senMem=sensors.append();
            if(!memCheck(senMem))
            {
                return false;
            }
            IRLickPort* newS=new (senMem) IRLickPort(params, myComm);            
//             sensors.append(new IRLickPort(params, myComm));
            return true;
        }

        if (strcmp(type, "cap1188") == 0)
        {
            debug("creating new cap1188 sensor");
            char *senMem=sensors.append();
            if(!memCheck(senMem))
            {
                return false;
            }
            Cap1188* newS=new (senMem) Cap1188(params, myComm);            
//             sensors.append(new Cap1188(params, myComm));

            if (params.containsKey("mapping"))
            {
                JsonArray& mapping = params.get<JsonArray>("mapping");
                for (int j = 0; j < mapping.size(); j++)
                {
                    JsonObject& mapping_params = mapping.get<JsonObject>(j);
                    char *senMem=sensors.append();
                    if(!memCheck(senMem))
                    {
                        return false;
                    }
                    Cap1188Mapping* newM=new (senMem) Cap1188Mapping(
                            mapping_params, newS, myComm);
                }
            }
            return true;
        }
    }
    else
    {
        debug("creating new simple sensor");
        char *senMem=sensors.append();
        if(!memCheck(senMem))
        {
            return false;
        }
        Sensor* newS=new (senMem) Sensor(params, myComm);        
//         sensors.append(new Sensor(params, myComm));
    }

    return true;
}

int SensorManager::getSize()
{
    return sensors.length();
}

void SensorManager::detect()
{
    for (int i = 0; i < sensors.length(); i++)
    {
        sensors.at(i)->detect();
    }
}

Sensor* SensorManager::at(int pin)
{
    for (int i=0; i < sensors.length(); i++)
    {
        if (sensors.at(i)->getPin() == pin)
        {
            return sensors.at(i);
        }
    }

    return NULL;
}

void SensorManager::remove(int pin)
{
    for (int i=0; i < sensors.length(); i++)
    {
        if (sensors.at(i)->getPin() == pin)
        {
            sensors.remove(i);
            debug("removed existing");
        }
    }
}

bool SensorManager::stateAt(int pin)
{
    Sensor* sensor = at(pin);
    if (sensor != NULL)
    {
        return sensor->getState();
    }

    return false;
}

void SensorManager::debug(char* msg)
{
    if (myComm != NULL)
    {
        myComm->debugMessage(msg);
    }
}

bool SensorManager::memCheck(char* mem)
{
    if(mem==nullptr)
    {
        debug("maximum number of sensors reached");
        myComm->errorMessage("maximum number of sensors reached");
        return false;
    }
    return true;
}
