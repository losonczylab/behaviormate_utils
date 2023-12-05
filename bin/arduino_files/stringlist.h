#ifndef h_stringlist
#define h_stringlist

#include <Arduino.h>

#include "simplelist.h"

class StringList
{
public:
    StringList();
    void append(char c);
    int size();
    char* assemble();

private:
    SimpleList<char> list;
};

#endif
