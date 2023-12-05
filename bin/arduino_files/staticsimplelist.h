#ifndef h_staticsimplelist
#define h_staticsimplelist

#include <Arduino.h>

template <class T, unsigned int maxLen, unsigned int memSize>
class StaticSimpleList
{
public:
    StaticSimpleList()
    {
        logicalLength = 0;
        maxLength=maxLen;
        
        for(int i=0; i<maxLength; i++)
        {
            memAvailable[i]=true;
        }
    }

    ~StaticSimpleList()
    {
    }

    char* append()
    {
        if (logicalLength >= maxLength)
        {
            return nullptr;
        }
        
        for(int i=0; i<maxLength; i++)
        {
            if(memAvailable[i])
            {
                memAvailable[i]=false;
                indList[logicalLength]=i;
                logicalLength++;
                
                return memList[i];
            }
        }
    }

    void remove(unsigned int index)
    {
        if(index>=logicalLength)
        {
            return;
        }
        memAvailable[indList[index]]=true;
        for (int i = index; i < logicalLength-1; i++)
        {
            indList[i] = indList[i+1];
        }
        logicalLength--;
    }

    T* at(unsigned int index)
    {
        return (T*)memList[indList[index]];
    }

    int length()
    {
        return logicalLength;
    }

private:
    unsigned int indList[maxLen];
    char memList[maxLen][memSize];
    bool memAvailable[maxLen];
    
    unsigned int maxLength;
    
    unsigned int logicalLength;
};

#endif
