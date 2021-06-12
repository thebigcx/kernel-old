#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>

void printf(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    char* str = malloc(strlen(format));
    vsprintf(str, format, list);
    // TODO: write to /dev/stdout
    free(str);
}