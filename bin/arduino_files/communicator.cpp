#include "communicator.h"

Communicator::Communicator(int baudRate, bool debug, int addr)
{
    this->debug = debug;
    Serial.begin(baudRate);
    nc = new (ncMem) NetComm(addr);
}

void Communicator::sendMessage(const char* msg)
{
    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["debug"] = msg;
    sendMessage(json);
}

void Communicator::debugMessage(const char* msg)
{
    if (debug)
    {
        sendMessage(msg);
    }
}

void Communicator::errorMessage(const char* msg)
{
    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& json = jsonBuffer.createObject();
    json["error"] = msg;
    sendMessage(json);
}

void Communicator::sendMessage(JsonObject& json)
{
    json["millis"] = millis();
    //json.printTo(Serial);
    //Serial.println();
    json.printTo(this->message_buffer);
    nc->sendMessage(this->message_buffer);
}

void Communicator::debugMessage(JsonObject& json)
{
    if (debug)
    {
        sendMessage(json);
    }
}

void Communicator::sendMessage(const char* key, int msg)
{
    StaticJsonBuffer<1024> jsonBuffer;
    JsonObject& message = jsonBuffer.createObject();
    message[key] = msg;
    message["millis"] = millis();
    //message.printTo(Serial);
    //Serial.println();

    message.printTo(this->message_buffer);
    nc->sendMessage(this->message_buffer);
}

void Communicator::sendMessage(
    const char* type,
    const char* key,
    const char* msg)
{
    StaticJsonBuffer<512> jsonBuffer;
    JsonObject& message = jsonBuffer.createObject();
    JsonObject& submessage = jsonBuffer.createObject();
    submessage[key] = msg;
    message["millis"] = millis();
    message[type] = submessage;

    message.printTo(this->message_buffer);
    nc->sendMessage(this->message_buffer);
}

void Communicator::debugMessage(const char* key, int msg)
{
    if (debug)
    {
        sendMessage(key, msg);
    }
}

bool Communicator::readSerial(int maxChars)
{
    qBuffer=messageQueue.getNextAvailable();
    if(qBuffer==nullptr)
    {
        sendMessage("queue is full");
        return false;
    }
    
    if(nc->update(qBuffer))
    {
        messageQueue.enqueue();
        return true;
    }
    return false;
    
//     char msg_buffer[1024];
//     if (nc->update(msg_buffer))
//     {
//         Serial.println(msg_buffer);
//         Serial.println(strlen(msg_buffer));
//         char* msg = new char[strlen(msg_buffer)];
//         strcpy(msg, msg_buffer);
//         if (!addToQueue(msg))
//         {
//             sendMessage("queue is full");
//         }
//         return true;
//     }
//     return false;
}


bool Communicator::action(JsonObject& messageJson)
{
    if (!messageJson.containsKey("action"))
    {
        return false;
    }

    const char* action = messageJson["action"].asString();
    if (strcmp(action, "test") == 0)
    {
        sendMessage("communicator", "test", "passed");
    }
    else if (strcmp(action, "debug_mode") == 0)
    {
        this->debug = !(this->debug);
    }
    else if (strcmp(action, "reset") == 0)
    {
        delay(20);
        REQUEST_EXTERNAL_RESET;
    }

    #ifdef VERSION
    else if (strcmp(action,"info") == 0)
    {
        StaticJsonBuffer<500> jsonBuffer;
        JsonObject& json = jsonBuffer.createObject();
        JsonObject& json_nested = jsonBuffer.createObject();

        #ifdef GIT_REVISION
        json_nested["revision"] = GIT_REVISION;
        #endif

        json_nested["version"] = VERSION;
        json["communicator"] = json_nested;
        sendMessage(json);
    }
    #endif

    return true;
}


// bool Communicator::addToQueue(char* message)
// {
//     TODO: apply error checking
//     return messageQueue.enqueue(message);
// }


bool Communicator::hasMessage()
{
    return (messageQueue.length() > 0);
}


char* Communicator::getMessage()
{
    return messageQueue.inspect();
}

void Communicator::dequeueMessage()
{
    messageQueue.dequeue();
}

void Communicator::reset_nc(int addr)
{
    nc->reset(addr);
}
