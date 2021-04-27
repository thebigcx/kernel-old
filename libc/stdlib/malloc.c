#include "stdlib.h"

extern void* _malloc();

void* malloc(size_t n)
{
    return _malloc(n);
}