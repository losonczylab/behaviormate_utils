#include "valve.h"

Valve::Valve() {}

Valve::Valve(int pin, Communicator* aCommunicator)
{
    this->pinNumber = pin;
    this->communicator = aCommunicator;

    this->openedAt = -1;
    this->duration = 0;

    pinMode(this->pinNumber, OUTPUT);
    digitalWrite(this->pinNumber, LOW);

    initMessages();
}

void Valve::initMessages()
{
    JsonObject& open_subjson = this->jsonBuffer.createObject();
    open_subjson["pin"] = this->pinNumber;
    open_subjson["action"] = "open";
    this->open_json["valve"] = open_subjson;

    JsonObject& close_subjson = this->jsonBuffer.createObject();
    close_subjson["pin"] = this->pinNumber;
    close_subjson["action"] = "close";
    this->close_json["valve"] = close_subjson;

    JsonObject& create_subjson = this->jsonBuffer.createObject();
    create_subjson["pin"] = this->pinNumber;
    create_subjson["action"] = "created";
    JsonObject& create_json = this->jsonBuffer.createObject();
    create_json["valve"] = create_subjson;
    this->communicator->sendMessage(create_json);
}


bool Valve::isOpen()
{
    return (this->openedAt >= 0);
}


void Valve::open(int time)
{
    this->openedAt = millis();
    digitalWrite(this->pinNumber, HIGH);
    communicator->sendMessage(this->open_json);
    this->duration = time;
    if ((time <= MAX_WAIT) && (time >= 0))
    {
        delay(time);
        close();
    }
}


void Valve::check()
{
    if ((this->isOpen()) && (millis() - this->openedAt >= this->duration))
    {
        close();
    }
}


void Valve::close()
{
    if (this->isOpen())
    {
        digitalWrite(this->pinNumber, LOW);
        communicator->sendMessage(this->close_json);
        this->openedAt = -1;
    }
}


int Valve::getPin()
{
    return this->pinNumber;
}


void Valve::debug(char* msg)
{
    if (this->communicator)
    {
        this->communicator->sendMessage(msg);
    }
}


void Valve::debug(int val)
{
    if (this->communicator)
    {
        char buf[30];
        this->communicator->sendMessage(itoa(val, buf, 10));
    }
}
