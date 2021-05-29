#include "string.h"

size_t strlen(const char* str)
{
    size_t s = 0;

    while (str[s] != '\0') s++;

    return s;
}