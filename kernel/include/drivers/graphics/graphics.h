#pragma once

#include "stdint.h"

// Bitmap fonts
typedef struct
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t c_size; // Character size
} psf1_header;

typedef struct
{
    psf1_header* header;
    void* glyph_buf;
} psf1_font;

typedef struct
{
    uint64_t fb_adr;
    uint32_t pix_per_line;
    uint32_t v_res;
    psf1_font* font;
} graphics_data_t;

extern graphics_data_t graphics_data;

void gr_clear();
void gr_clear_color(float r, float g, float b, float a);

void _putchar(char c);