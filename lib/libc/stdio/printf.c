#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void printf(const char* format, ...)
{
    va_list list;
    va_start(list, format);
    char* str = malloc(strlen(format));
    vsnprintf(str, format, list);
    write(stdout->fd, str, strlen(str));
    free(str);
}