#pragma once

#include <stdint.h>

typedef struct mouse_pkt
{
    int8_t btns;
    int8_t xmov;
    int8_t ymov;
    int8_t scroll;

} mspkt_t