#include "sensor.h"

Cap1188Mapping::Cap1188Mapping(JsonObject& params,
                               Cap1188* aCap1188,
                               Communicator* aCommunicator)
{
    this->communicator = aCommunicator;
    this->cap1188 = aCap1188;

    if (params.containsKey("pin"))
    {
        this->pin = params["pin"].as<int>();
    }
    else
    {
        this->error("Cap1188 Mapping requires pin assignment to setup");
        return;
    }

    if (params.containsKey("sensor"))
    {
        this->sensor = params["sensor"].as<int>();
    }
    else
    {
        this->error("Cap1188 Mapping requires sensor assignment to setup");
        return;
    }

    this->initMessages();
    this->status = false;
    this->state = false;
}


void Cap1188Mapping::initMessages()
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


bool Cap1188Mapping::detect()
{
    bool sensor_status = this->cap1188->getSensorStatus() == this->sensor;

    this->state = false;
    if ((!status) && (sensor_status))
    {
        state = true;
        status = true;
        communicator->sendMessage(start_json);
        return true;
    }
    else if ((status) && (!sensor_status))
    {
        communicator->sendMessage(stop_json);
        status = false;
    }

    return false;
}
