#include "linkedstring.h"

LinkedString::LinkedString()
{
    strStart = NULL;
    strEnd = NULL;

    length = 0;
}

int LinkedString::getLength()
{
    return length;
}

void LinkedString::append(char c)
{
    charNode* newNode = new charNode[sizeof(charNode)];
    newNode->character = c;
    newNode->next = NULL;

    length++;
    if (strStart == NULL)
    {
        strStart = newNode;
        strEnd = newNode;
    }
    else
    {
        strEnd->next = newNode;
        strEnd = strEnd->next;
    }
}

char* LinkedString::assemble()
{
    charNode* ptr;
    ptr = strStart;

    char* assembled = new char[length+1];
    int idx = 0;
    while (ptr != NULL)
    {
        assembled[idx] = ptr->character;
        idx++;
        ptr = ptr->next;
    }
    assembled[length] = '\0';

    return assembled;
}
