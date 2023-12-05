#ifndef h_inverted_valve
#define h_inverted_valve

#include <Arduino.h>
#include <ArduinoJson.h>

#include "communicator.h"

class InvertedValve : public Valve
{

public:
   InvertedValve(int pin, Communicator* aCommunicator=NULL);

   void open(int time=-1);

   void close();

protected:
   void initMessages();

};

#endif
