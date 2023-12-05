#include "stringlist.h"

StringList::StringList()
{

}

void StringList::append(char c)
{
    list.append(c);
}

int StringList::size()
{
    return list.size();
}

char* StringList::assemble()
{
    int currSize = size();
    char* assembled = new char[currSize+1];
    for (int i = 0; i < currSize; i++)
    {
        assembled[i] = list.at(i);
    }
    assembled[currSize] = '\0';

    return assembled;
}
