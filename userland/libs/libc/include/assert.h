#pragma once

#include <stdio.h>
#include <stdlib.h>

void __assert_failed(const char* expr)
{
    printf("Assertion failed: %s\n", expr);
    abort();
}

#define assert(expr) if (!(expr)) __assert_failed(#expr);