#include "tone.h"

Tone::Tone(int pin, int frequency, Communicator* aCommunicator) : Valve()
{
    this->pinNumber = pin;
    this->frequency = frequency;
    this->communicator = aCommunicator;

    openedAt = -1;
    duration = 0;

    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, LOW);

    initMessages();
}

Tone::Tone(int pin, Communicator* aCommunicator) : Valve()
{
    this->pinNumber = pin;
    this->frequency = -1;
    this->communicator = aCommunicator;

    openedAt = -1;
    duration = 0;

    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, LOW);

    initMessages();
}

void Tone::initMessages()
{
    JsonObject& open_subjson = this->jsonBuffer.createObject();
    open_subjson["pin"] = pinNumber;
    open_subjson["action"] = "open";
    open_subjson["frequency"] = this->frequency;
    this->open_json["tone"] = open_subjson;

    JsonObject& close_subjson = this->jsonBuffer.createObject();
    close_subjson["pin"] = pinNumber;
    close_subjson["action"] = "close";
    this->close_json["tone"] = close_subjson;

    JsonObject& create_subjson = this->jsonBuffer.createObject();
    create_subjson["pin"] = pinNumber;
    create_subjson["action"] = "created";
    create_subjson["frequency"] = this->frequency;
    JsonObject& create_json = this->jsonBuffer.createObject();
    create_json["tone"] = create_subjson;
    communicator->sendMessage(create_json);
}

void Tone::open(int time)
{
    this->openedAt = millis();
    tone(this->pinNumber, this->frequency);
    this->open_json["tone"]["frequency"] = this->frequency;
    communicator->sendMessage(this->open_json);
    this->duration = time;

    // TODO: remove this check? probably should never delay for short times
    if ((time <= MAX_WAIT) && (time >= 0))
    {
        delay(time);
        close();
    }
}

void Tone::open(int time, int frequency)
{
    this->openedAt = millis();
    tone(this->pinNumber, frequency);
    this->open_json["tone"]["frequency"] = frequency;
    communicator->sendMessage(this->open_json);
    this->duration = time;

    // TODO: remove this check? probably should never delay for short times
    if ((time <= MAX_WAIT) && (time >= 0))
    {
        delay(time);
        close();
    }
}

void Tone::close()
{
    noTone(this->pinNumber);
    communicator->sendMessage(close_json);
    openedAt = -1;
}
