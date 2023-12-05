#include "irport.h"

IRLickPort::IRLickPort(JsonObject& params,
         Communicator* aCommunicator) : Sensor()
{
    this->communicator = aCommunicator;
    if (!params.containsKey("pin"))
    {
        debug("lick port requires pin");
        return;
    }

    this->pin = params["pin"].as<int>();
    //pinMode(this->pin, INPUT);

    this->report_pin = -1;
    if (params.containsKey("report_pin"))
    {
        this->report_pin = params["report_pin"].as<int>();
        pinMode(this->report_pin, OUTPUT);
        digitalWrite(this->report_pin, LOW);
    }

    if (params.containsKey("lick_threshold"))
    {
        this->lick_threshold = ((int) (params["lick_threshold"].as<float>()) * 102.4);
    }
    else
    {
        this->lick_threshold = 0.75;
    }

    if (params.containsKey("debug"))
    {
        this->debug_mode = 1000;
    }
    else
    {
        this->debug_mode = -1;
    }


    this->state = false;
    this->in_lick = false;
    initMessages();
}

void IRLickPort::initMessages()
{
    JsonObject& start_json = this->jsonBuffer.createObject();
    JsonObject& start_subjson = this->jsonBuffer.createObject();
    start_subjson["pin"] = pin;
    start_subjson["action"] = "start";
    this->start_json["lick"] = start_subjson;

    JsonObject& stop_json = this->jsonBuffer.createObject();
    JsonObject& stop_subjson = this->jsonBuffer.createObject();
    stop_subjson["pin"] = pin;
    stop_subjson["action"] = "stop";
    this->stop_json["lick"] = stop_subjson;

    StaticJsonBuffer<512> createBuffer;
    JsonObject& create_subjson = createBuffer.createObject();
    create_subjson["pin"] = pin;
    create_subjson["lick_threshold"] = this->lick_threshold;
    create_subjson["action"] = "created";
    JsonObject& create_json = createBuffer.createObject();
    create_json["lick"] = create_subjson;
    this->communicator->sendMessage(create_json);
}

bool IRLickPort::detect()
{
    this->state = false;

    int value = analogRead(this->pin);
    if (this->debug_mode == 0)
    {
        char buf[64];
        sprintf(buf, "Optical Lick Sensor: %d", value);
        this->communicator->sendMessage(buf);
    }
    else if (this->debug_mode > 0)
    {
        this->debug_mode--;
    }

    if (!this->in_lick && (value < this->lick_threshold))
    {
        this->state = true;
        this->in_lick = true;
        this->communicator->sendMessage(this->start_json);
        return true;
    }
    else if (this->in_lick && (value > this->lick_threshold))
    {
        this->in_lick = false;
        this->communicator->sendMessage(this->stop_json);
    }

    return false;
}
