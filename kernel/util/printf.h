#pragma once

#include <util/stdlib.h>

int vsnprintf(char* str, const char* format, va_list arg);
int snprintf(char* s, size_t n, const char* format, ...);