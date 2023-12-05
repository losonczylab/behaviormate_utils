#include "sensor.h"

Sensor::Sensor() {}

Sensor::Sensor(int aPin,
               Communicator* aCommunicator)
{
    pin = aPin;
    communicator = aCommunicator;

    initMessages();
    pinMode(pin, INPUT_PULLUP);
}


Sensor::Sensor(JsonObject& params,
               Communicator* aCommunicator)
{
    communicator = aCommunicator;

    if (params.containsKey("pin"))
    {
        this->pin = params["pin"].as<int>();
        initMessages();
        pinMode(this->pin, INPUT_PULLUP);
    }
    else
    {
        debug("sensor requries pin to setup");
    }
}

bool Sensor::getState()
{
    return state;
}

void Sensor::initMessages()
{
    JsonObject& start_json = this->jsonBuffer.createObject();
    JsonObject& start_subjson = this->jsonBuffer.createObject();
    start_subjson["pin"] = pin;
    start_subjson["action"] = "start";
    this->start_json["sensor"] = start_subjson;

    JsonObject& stop_json = this->jsonBuffer.createObject();
    JsonObject& stop_subjson = this->jsonBuffer.createObject();
    stop_subjson["pin"] = pin;
    stop_subjson["action"] = "stop";
    this->stop_json["sensor"] = stop_subjson;

    JsonObject& create_subjson = this->jsonBuffer.createObject();
    create_subjson["pin"] = pin;
    create_subjson["action"] = "created";
    JsonObject& create_json = this->jsonBuffer.createObject();
    create_json["sensor"] = create_subjson;
    communicator->sendMessage(create_json);
}

int Sensor::getPin()
{
    return pin;
}

bool Sensor::detect()
{
    state = false;
    if ((!status) && (digitalRead(pin) == HIGH))
    {
        state = true;
        status = true;
        communicator->sendMessage(start_json);
        return true;
    }
    else if ((status) && (digitalRead(pin) == LOW))
    {
        communicator->sendMessage(stop_json);
        status = false;
    }

    return false;
}

void Sensor::debug(char* msg)
{
    if (communicator != NULL)
    {
        communicator->debugMessage(msg);
    }
}

void Sensor::error(char* msg)
{
    if (communicator != NULL)
    {
        communicator->errorMessage(msg);
    }
}
