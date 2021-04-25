#pragma once

#include "stdint.h"

typedef struct
{
    uint64_t fb_adr;
    uint32_t pix_per_line;
    uint32_t v_res;
} graphics_data_t;

extern graphics_data_t graphics_data;

void gr_clear();
void gr_clear_color(float r, float g, float b, float a);