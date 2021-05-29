#include <stdio.h>
#include <stdarg.h>

void printf(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    char* str = malloc(strlen(format));
    vsprintf(str, format, list);
    puts(str); // TODO: output stream files
    free(str);
}