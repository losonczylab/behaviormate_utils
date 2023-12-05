#ifndef h_communicator
#define h_communicator

#include <Arduino.h>
#include <ArduinoJson.h>

#include "simplearrayqueue.h"
#include "netcomm_due.h"

#define END_CHAR '\n'

#define DEFAULT_MAX_READ 500
#define QUEUE_LENGTH 15
#define SYSRESETREQ    (1<<2)
#define VECTKEY        (0x05fa0000UL)
#define VECTKEY_MASK   (0x0000ffffUL)
#define AIRCR          (*(uint32_t*)0xe000ed0cUL) // fixed arch-defined address
#define REQUEST_EXTERNAL_RESET (AIRCR=(AIRCR&VECTKEY_MASK)|VECTKEY|SYSRESETREQ)


typedef char* string;

class Communicator
{
public:
    Communicator(int baudRate=9600, bool debug=false, int addr=0);

    void sendMessage(const char* msg);
    void debugMessage(const char* msg);
    void errorMessage(const char* msg);

    void sendMessage(const char* key, int msg);
    void debugMessage(const char* key, int msg);

    void sendMessage(const char* type, const char* key, const char* msg);

    void sendMessage(JsonObject& json);
    void debugMessage(JsonObject& json);

    // Read spefied amount of data from the serial port and populate
    // the message queue
    bool readSerial(int maxChars=DEFAULT_MAX_READ);

    // Returns true, if a message has been completed and queued
    bool hasMessage();

    // Get a message from the message queue
    char* getMessage();

    //remove the first message in the queue
    void dequeueMessage();

    bool action(JsonObject& messageJson);

    void reset_nc(int addr=0);

private:
    bool debug;
//     bool addToQueue(char* message);
    char message_buffer[1024];

    char* qBuffer;

    SimpleArrayQueue<char, QUEUE_LENGTH, 1024> messageQueue;
    
    NetComm *nc;
    char ncMem[sizeof(NetComm)];
};

#endif
