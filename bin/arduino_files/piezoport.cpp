#include "piezoport.h"

LickPortPiezo::LickPortPiezo(JsonObject& params,
         Communicator* aCommunicator) : Sensor()
{
    this->communicator = aCommunicator;
    if (!params.containsKey("pin"))
    {
        this->error("peizo lick port requires pin");
        return;
    }

    this->pin = params["pin"].as<int>();
    pinMode(this->pin, INPUT);
    digitalWrite(this->pin, HIGH); // enable pullup resistor

    this->report_pin = -1;
    if (params.containsKey("report_pin"))
    {
        this->report_pin = params["report_pin"].as<int>();
        pinMode(this->report_pin, OUTPUT);
        digitalWrite(this->report_pin, LOW);
    }

    if (params.containsKey("lick_threshold"))
    {
        this->lick_threshold = params["lick_threshold"].as<float>() * 0.31;
    }
    else if (params.containsKey("lick_threashold"))
    {
        this->lick_threshold = params["lick_threashold"].as<float>() * 0.31;
    }
    else
    {
        this->error("peizo lick port requires lick_threshold");
        return;
    }

    if (params.containsKey("lick_duration"))
    {
        this->lick_duration = params["lick_duration"].as<int>();
    }
    else
    {
        this->error("peizo lick port requires lick_duration");
        return;
    }

    if (params.containsKey("min_interval"))
    {
        this->min_interval = params["min_interval"].as<int>();
    }
    else
    {
        this->min_interval = 200;
    }

    ///Tunable parameters:
    // *** lick_threshold = 5; //
    // *** min_interval =  200; //in ms
    // *** lick_duration = 50; //in ms

    this->state = false;
    this->prev_value = 0;
    this->prev_time = 0;
    this->prev_lick = 0;

    initMessages();
}

void LickPortPiezo::initMessages()
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

    JsonObject& create_subjson = this->jsonBuffer.createObject();
    create_subjson["pin"] = pin;
    create_subjson["lick_threshold"] = this->lick_threshold;
    create_subjson["lick_duration"] = this->lick_duration;
    create_subjson["min_interval"] = this->min_interval;
    create_subjson["action"] = "created";
    JsonObject& create_json = this->jsonBuffer.createObject();
    create_json["lick"] = create_subjson;
    this->communicator->sendMessage(create_json);

    this->licked = true;
}

bool LickPortPiezo::detect()
{
    unsigned long time = millis();
    unsigned long delta_t = time - prev_time;

    this->state = false;
    if (delta_t <= 5)
    {
        return false;
    }

    int lick_interval = time - this->prev_lick;
    if (lick_interval <= lick_duration)
    {
        return false;
    }

    this->prev_time = time;
    float value = float(analogRead(this->pin));
    float delta_v = (value - this->prev_value)/((float) delta_t);
    this->prev_value = value;

    if ((delta_v > this->lick_threshold ) && (lick_interval >= this->min_interval))
    {
        if (this->report_pin != -1)
        {
            digitalWrite(this->report_pin, HIGH);
        }

        this->prev_lick = time;
        communicator->sendMessage(this->start_json);
        this->licked = true;
        this->state = true;
        return true;
    }

    if (this->licked)
    {
        this->licked = false;
        if (this->report_pin != -1)
        {
            digitalWrite(this->report_pin, LOW);
        }
        communicator->sendMessage(this->stop_json);
    }

    return false;
}
