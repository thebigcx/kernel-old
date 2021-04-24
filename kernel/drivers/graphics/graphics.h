#pragma once

#include <stdint.h>

typedef struct
{
    uint64_t fb_adr;
    uint32_t pix_per_line;
} graphics_data_t;

extern graphics_data_t graphics_data;

void init_graphics();