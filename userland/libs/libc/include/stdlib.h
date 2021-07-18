#pragma once

#include <stddef.h>

#define RAND_MAX 0x7FFFFFFF

void* malloc(size_t size);
void free(void* ptr);
void* realloc(void* ptr, size_t size);
void* calloc(size_t nitems, size_t size);

char* itoa(int value, char* str, int base);
char* ultoa(unsigned long n, char* str, int base);
int abs(int n);

int rand();
void srand(unsigned int seed);

void abort();