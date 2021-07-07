#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void vfprintf(FILE* file, const char* format, va_list args)
{
    //char* str = malloc(strlen(format));
    // TODO: userspace malloc/free
    char str[100];
    vsnprintf(str, format, args);
    write(file->fd, str, strlen(str));
    //free(str);
    
}

void fprintf(FILE* file, const char* format, ...)
{
    va_list list;
    va_start(list, format);
    
    vfprintf(file, format, list);

    va_end(list);
}

void printf(const char* format, ...)
{
    va_list list;
    va_start(list, format);

    vfprintf(stdout, format, list);

    va_end(list);
}