#include "inverted_valve.h"

InvertedValve::InvertedValve(int pin, Communicator* aCommunicator) : Valve()
{
    this->pinNumber = pin;
    this->communicator = aCommunicator;

    openedAt = -1;
    duration = 0;

    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, HIGH);

    initMessages();
}

void InvertedValve::initMessages()
{
    JsonObject& open_subjson = this->jsonBuffer.createObject();
    open_subjson["pin"] = pinNumber;
    open_subjson["action"] = "open";
    this->open_json["valve"] = open_subjson;

    JsonObject& close_subjson = this->jsonBuffer.createObject();
    close_subjson["pin"] = pinNumber;
    close_subjson["action"] = "close";
    this->close_json["valve"] = close_subjson;

    JsonObject& create_subjson = this->jsonBuffer.createObject();
    create_subjson["pin"] = pinNumber;
    create_subjson["action"] = "created";
    create_subjson["inverted"] = true;
    JsonObject& create_json = this->jsonBuffer.createObject();
    create_json["valve"] = create_subjson;
    communicator->sendMessage(create_json);
}

void InvertedValve::open(int time)
{
    this->openedAt = millis();
    digitalWrite(this->pinNumber, LOW);
    communicator->sendMessage(this->open_json);
    this->duration = time;

    // TODO: remove this check? probably should never delay for short times
    if ((time <= MAX_WAIT) && (time >= 0))
    {
        delay(time);
        close();
    }
}

void InvertedValve::close()
{
    digitalWrite(this->pinNumber, HIGH);
    communicator->sendMessage(close_json);
    openedAt = -1;
}
