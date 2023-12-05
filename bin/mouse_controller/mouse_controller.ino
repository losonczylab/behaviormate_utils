#include <ArduinoJson.h>
#include "communicator.h"

#include <MouseController.h>
#include <Usb.h>

USBHost usb;
MouseController mouse(usb);
Communicator* comm;
float deltaX;
float deltaY;
int counter;
int lastSend;
int sendDelay;

StaticJsonBuffer<128> jsonBuffer;
JsonObject& message;
JsonObject& value;

void mouseMoved() {
    deltaX += mouse.getXChange();
    deltaY += mouse.getYChange();
}

void setup()
{
    message = jsonBuffer.createObject();
    value = message.createNestedObject("position");

    deltaX = 0;
    deltaY = 0;
    counter = 0;
    lastSend = 0;
    sendDelay = 50;
    
    comm = new Communicator();
    comm->sendMessage("starting");
}

void loop()
{
    usb.Task();
    int ltime = millis();
    if (ltime > (lastSend + sendDelay))
    {
      if ((deltaX != 0) || (deltaY != 0)) {
        value["dx"] = deltaX;
        value["dy"] = deltaY;
        message["millis"] = millis();
        comm->sendMessage(message);

        deltaX = 0;
        deltaY = 0;
        lastSend = ltime;
      }
    }
}
