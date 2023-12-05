#ifndef h_linkedstring
#define h_linkedstring

#include "Arduino.h"

struct charNode
{
    char character;
    struct charNode* next;
};

class LinkedString
{
public:
    LinkedString();
    int getLength();

    void append(char c);
    char* assemble();

private:
    charNode* strStart;
    charNode* strEnd;

    int length;
};

#endif
