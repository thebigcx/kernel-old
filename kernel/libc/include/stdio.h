#pragma once

#include <stddef.h>
#include <stdarg.h>
#include <stdint.h>

typedef struct fs_file_t FILE;

void putchar(char c);
void puts(const char* str);
int sprintf(char* str, const char* format, ...);
int vsprintf(char* str, const char* format, va_list arg);
void printf(const char* format, ...);
FILE* fopen(const char* path, const char* mode);
int fclose(FILE* stream);
size_t fread(void* ptr, size_t size, size_t nmemb, FILE* stream);
size_t fwrite(const void* ptr, size_t size, size_t nmemb, FILE* stream);