#pragma once

#include <util/types.h>
#include <util/printf.h>

void _assertion_failed(const char* expr, const char* file, uint32_t line)
{
    char msg[256];
    snprintf(msg, 256, "Kernel assertion failed: '%s', file %s, line %d", expr, file, line);
    panic(msg);
}

#define KASSERT(expr) if (!(expr)) _assertion_failed(#expr, __FILE__, __LINE__);
