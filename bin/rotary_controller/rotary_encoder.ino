#include <ArduinoJson.h>
#include "communicator.h"

#include <MouseController.h>
#include <Usb.h>

USBHost usb;
MouseController mouse(usb);
Communicator* comm;

float prev_y;
int counter;
int encoderPin = 1;

void setup()
{
    prev_y = 0;
    counter = 0;
    
    comm = new Communicator(115200);
    comm->sendMessage("starting");
}

void loop()
{
    float current_y = analogRead(encoderPin);
    float delta_y = current_y - prev_y;
    prev_y = current_y;

    if ( delta_y != 0 ) 
    {
      StaticJsonBuffer<128> jsonBuffer;
      JsonObject& json = jsonBuffer.createObject();
      json["x"] = 0;
      json["y"] = delta_y;
      comm->sendMessage(json);

    }
}
