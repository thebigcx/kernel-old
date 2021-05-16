#include <stdio.h>
#include <stdarg.h>

void printf(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    //char* str = malloc(strlen(format));
    char str[100];
    vsprintf(str, format, list);
    puts(str); // TODO: output stream files
    //free(str);
}