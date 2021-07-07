#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

typedef struct FILE
{
    int fd;

} FILE;

extern FILE* stdout;
extern FILE* stdin;
extern FILE* stderr;

void putchar(char c);
void puts(const char* str);
int sprintf(char* str, const char* format, ...);
int vsnprintf(char* str, const char* format, va_list args);
void printf(const char* format, ...);
void fprintf(FILE* file, const char* format, ...);
void vfprintf(FILE* file, const char* format, va_list args);
FILE* fopen(const char* path, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);