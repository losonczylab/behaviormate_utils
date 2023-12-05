#ifndef h_simplequeue
#define h_simplequeue

#include <Arduino.h>

template <class T, int qLen>
class SimpleQueue
{
public:
    SimpleQueue()
    {
        maxLength = qLen;
        idx = 0;
        top = 0;
    }

    bool enqueue(T aValue)
    {
        int next = (idx+1)%maxLength;
        if (next == top)
        {
            return false;
        }

        queue[idx] = aValue;
        idx = next;
        return true;
    }

    T dequeue()
    {
        T value = queue[top];
        top = (top+1)%maxLength;
        return value;
    }

    T inspect()
    {
        return queue[top];
    }

    int length()
    {
        int adjIdx = idx;
        if (adjIdx < top)
        {
            adjIdx += maxLength;
        }

        return adjIdx-top;
    }

    bool hasNext()
    {
        return (idx != top);
    }

private:
    int idx;
    int top;
    T queue[qLen];
    int maxLength;

};

#endif

