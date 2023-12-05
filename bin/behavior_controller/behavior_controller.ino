#include <Arduino.h>
#include <ArduinoJson.h>
#include <Wire.h>


#include "op_new.h"
#include "stringlist.h"
#include "communicator.h"
#include "simplearrayqueue.h"
#include "lickport.h"
#include "mprvalues.h"
#include "valvemanager.h"
#include "valve.h"
#include "contextmanager.h"
#include "pulsedcontext.h"
#include "operantcontext.h"
#include "rfidreader.h"
#include "sensormanager.h"

#define SDA_PIN 20
#define SCL_PIN 21

static char commMem[sizeof(Communicator)];
Communicator* communicator;

static char valveMMem[sizeof(ValveManager)];
ValveManager* valveManager;

static char sensorMMem[sizeof(SensorManager)];
SensorManager* sensorManager;

static char ctxtMMem[sizeof(ContextManager)];
ContextManager* contextManager;

void debug(char* msg)
{
    if (communicator != NULL)
    {
        communicator->sendMessage(msg);
    }
}

void setup()
{
    Serial1.begin(9600);
    pinMode(51, INPUT);
    //pinMode(47, INPUT);
    int addr = digitalRead(51);
    communicator = new (commMem) Communicator(115200, false, addr);
    valveManager = new (valveMMem) ValveManager(communicator);
    sensorManager = new (sensorMMem) SensorManager(communicator);
    contextManager = new (ctxtMMem) ContextManager(
        valveManager, sensorManager, communicator);

    StaticJsonBuffer<256> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["starting"] = GIT_REVISION;
    communicator->sendMessage(json);
    jsonBuffer.clear();
}

void iterateJson(JsonObject& json)
{
    for (JsonObject::iterator it=json.begin(); it!= json.end(); ++it) {
        Serial.println(it->key);
        Serial.println(it->value.asString());
        Serial.println("-");
    }
}


bool parseMessage(JsonObject& messageJson)
{
    if (messageJson.containsKey("communicator"))
    {
        return communicator->action(messageJson["communicator"].as<JsonObject&>());
    }

    if (messageJson.containsKey("valves"))
    {
        valveManager->action(messageJson["valves"].as<JsonObject&>());
        return true;
    }

    if (messageJson.containsKey("contexts"))
    {
        contextManager->action(messageJson["contexts"].as<JsonObject&>());
        return true;
    }

    if (messageJson.containsKey("sensors"))
    {
        sensorManager->action(messageJson["sensors"].as<JsonObject&>());
        return true;
    }

    return false;
}

int last_message = -1;
void loop()
{
    communicator->readSerial();
    contextManager->update();
    sensorManager->detect();

    if (communicator->hasMessage())
    {
        char* msg = communicator->getMessage();
        //communicator->debugMessage(msg);

        StaticJsonBuffer<4112> jsonBuffer;
        JsonObject& message = jsonBuffer.parseObject(msg);
        if (!message.success())
        {
            communicator->sendMessage("nack");
        }
        else
        {
            message["id"] = (last_message+1);
            communicator->sendMessage("ack", message["id"]);

            if (message["id"].as<int>() == last_message)
            {
                // if this message is a duplicate transmission return without
                // parsing the message
                communicator->debugMessage("duplicate message");
            }
            bool success = parseMessage(message);
            if (success)
            {
                communicator->debugMessage("message understood");
            }
            else
            {
                communicator->sendMessage("message not understood");
                communicator->sendMessage(message);
            }
            last_message = message["id"].as<int>();
        }
        communicator->dequeueMessage();
    }

    valveManager->update();

    /*
    if (digitalRead(47) != 1)
    {
        Serial.println("RESETTING NC");
        communicator->reset_nc(digitalRead(51));
    }*/
}
