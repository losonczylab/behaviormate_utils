#ifndef h_simplearrayqueue
#define h_simplearrayqueue

#include <Arduino.h>

template <class T, int maxQLen, int arrayLen>
class SimpleArrayQueue
{
public:
    SimpleArrayQueue()
    {
        maxLength = maxQLen;
        maxArrLen = arrayLen;
        qStartInd = 0;
        qEndInd = 0;
        qNextInd=qEndInd;
    }
    
    T* getNextAvailable()
    {
        T* retPtr;
        
        qNextInd=(qEndInd+1)%maxLength;
        if(qNextInd==qStartInd)
        {
            qNextInd=qEndInd;
            return nullptr;
        }
        retPtr=queue[qEndInd];
        return retPtr;
    }
    
    void enqueue()
    {
        qEndInd=qNextInd;
    }

    void dequeue()
    {
        if(qStartInd == qEndInd)
        {
            return;
        }
        qStartInd = (qStartInd+1)%maxLength;
    }

    T* inspect()
    {
        if(qStartInd == qEndInd)
        {
            return nullptr;
        }
        return queue[qStartInd];
    }

    int length()
    {
        int  qEAdjInd = qEndInd;
        if (qEAdjInd < qStartInd)
        {
            qEAdjInd += maxLength;
        }

        return qEAdjInd-qStartInd;
    }

    bool hasNext()
    {
        return (qEndInd != qStartInd);
    }

private:
    int qEndInd;
    int qStartInd;
    int qNextInd;
    T queue[maxQLen][arrayLen];
    int maxLength;
    int maxArrLen;

};

#endif

 
