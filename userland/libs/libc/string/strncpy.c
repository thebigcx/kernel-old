#include <string.h>

char* strncpy(char* dst, const char* src, uint32_t n)
{
    while (n--)
    {
        *dst++ = *src++;
    }
    
    return dst;
}