#include "graphics.h"

graphics_data_t graphics_data;

static uint32_t clear_color = 0x00000000;

void gr_clear()
{
    for (uint32_t x = 0; x < graphics_data.pix_per_line; x++)
    for (uint32_t y = 0; y < graphics_data.v_res; y++)
    {
        *((uint32_t*)(graphics_data.fb_adr + 4 * graphics_data.pix_per_line * y + 4 * x)) = clear_color;
    }
}

// Layout ARGB - each channel 8-bit
void gr_clear_color(float r, float g, float b, float a)
{
    uint8_t br = (uint8_t)(r * 255.0f);
    uint8_t bg = (uint8_t)(g * 255.0f);
    uint8_t bb = (uint8_t)(b * 255.0f);
    uint8_t ba = (uint8_t)(a * 255.0f);

    clear_color = (ba << 24) |
                  (br << 16) |
                  (bg << 8) | bb;
}