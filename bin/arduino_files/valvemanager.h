#ifndef h_valvemanager
#define h_valvemanager

#include <Arduino.h>
#include <ArduinoJson.h>

#include "valve.h"
#include "tone.h"
#include "inverted_valve.h"
#include "staticsimplelist.h"
#include "communicator.h"


class ValveManager
{
public:
    ValveManager(Communicator* aComm=NULL);
    ~ValveManager();

    void action(JsonObject& json);
    void createValve(int pin);
    void openAll();
    void closeAll();
    void update();
    bool open(int pin, int duration=-1);
    bool open(int pin, int duration, int frequency);
    bool close(int pin);

private:
    StaticSimpleList<Valve, 20, sizeof(Tone)> myValves;
    Communicator* myComm;

    Valve* atPin(int pin);
    void debug(char* msg);
    void debug(int val);
    bool remove(int pin);
    bool memCheck(char *mem);
};

#endif
