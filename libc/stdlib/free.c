#include "stdlib.h"

extern void _free(void* ptr);

void free(void* ptr)
{
    _free(ptr);
}