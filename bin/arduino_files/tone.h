#ifndef h_tone
#define h_tone

#include <Arduino.h>
#include <ArduinoJson.h>

#include "communicator.h"

class Tone : public Valve
{

public:
   Tone(int pin, int frequency, Communicator* aCommunicator=NULL);
   Tone(int pin, Communicator* aCommunicator=NULL);

   void open(int time=-1);
   void open(int time, int frequency);
   void close();
   void setFrequency(int frequency);

protected:
    void initMessages();

private:
    int frequency;
};

#endif
