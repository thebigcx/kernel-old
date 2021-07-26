#pragma once

#include <util/types.h>

#define MACADDR_LEN 6

// Wrapper
typedef struct macaddr
{
    uint8_t val[MACADDR_LEN];

} macaddr_t;