#pragma once

#include <stddef.h>
#include <stdint.h>

void* memcpy(void* dst, const void* src, size_t n);
void* memmove(void* dst, const void* src, size_t n);
void* memset(void* dst, unsigned char c, size_t n);
int memcmp(const void* s1, const void* s2, size_t n);
size_t strlen(const char* str);
char* strrev(char* str);
int strcmp(const char* str1, const char* str2);
int strncmp(const char* str1, const char* str2, size_t n);
char* strcpy(char* dst, const char* src);
char* strncpy(char* dst, const char* src, uint32_t n);
char* strsep(char** str, const char* delim);