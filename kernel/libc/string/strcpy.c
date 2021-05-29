#include <string.h>

char* strcpy(char* dst, const char* src)
{
    char* ptr = dst;

    while (*src != '\0')
    {
        *dst = *src;
        dst++;
        src++;
    }

    *dst = '\0';

    return ptr;
}