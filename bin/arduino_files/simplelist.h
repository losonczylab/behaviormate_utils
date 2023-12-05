#ifndef h_simplelist
#define h_simplelist

#include <Arduino.h>

template <class T>
class SimpleList
{
public:
    SimpleList(int initLength=8)
    {
        list = new T[initLength];
        logicalSize = 0;
        reservedSize = initLength;
        deleteFunction = (*doNothing);
    }

    ~SimpleList()
    {
        for (int i = 0; i < logicalSize; i++)
        {
            deleteFunction(list[i]);
        }
        delete[] list;
    }

    void setDeleteFunc(void (*f)(T item))
    {
        deleteFunction = f;
    }

    void append(T item)
    {
        if (logicalSize == reservedSize)
        {
            reservedSize = reservedSize*2;
            list = (T*)realloc(list,reservedSize*sizeof(T));
        }

        list[logicalSize] = item;
        logicalSize++;
    }

    void remove(int index)
    {
        T temp = list[index];
        for (int i = index; i < logicalSize-1; i++)
        {
            list[i] = list[i+1];
        }
        logicalSize--;

        deleteFunction(temp);
    }

    T at(int index)
    {
        return list[index];
    }

    int size()
    {
        return logicalSize;
    }

    SimpleList<T> copy()
    {
        SimpleList<T> newList(logicalSize);
        for (int i = 0; i < size(); i++)
        {
            newList.append(at(i));
        }

        return newList;
    }

private:
    T* list;
    int logicalSize;
    int reservedSize;
    void (*deleteFunction)(T item);

    static void doNothing(T item)
    {
        (void) item;
    }
};

#endif
