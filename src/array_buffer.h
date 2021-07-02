#pragma once

#include <cstdio> // realloc

#ifndef SHORT_VARIABLES
    #define SHORT_VARIABLES
    typedef unsigned long long u64;
    typedef u64 umax;
#endif

#define PRINT_CONSOLE(message, ...) ((void)0)

struct DEBUG_Data
{

};

inline void StoreData_(void **data, umax sizeof_Data, void **memory)
{
    *data = *memory;
    *memory = (void*)((char*)*memory + sizeof_Data);
}

//#define STOREDATA(data, count, type, memory) (data) = (type*)(memory); (memory) = (void*)( (type*)(memory) + (count) );

#define STOREDATA(data, count, memory) StoreData_( (void **)(&data), ( (count) * sizeof(*data) ), (&memory) );

inline umax GrowData_(void **data, umax sizeof_Data)
{
    umax result = 0;

    char* new_Data = (char*)realloc(*data, (size_t)(sizeof_Data));
    if(new_Data)
    {
        result = sizeof_Data;
        *data = new_Data;
    }
    else if(*data)
    {
        // TODO handle memory issues
        PRINT_CONSOLE("realloc failed in GrowData_\n");
        ASSERT_NOW();
    }

    return result;
}

#define GROWDATA(data, count) GrowData_( (void **)(&data), ( (count) * sizeof(*data) ) );

