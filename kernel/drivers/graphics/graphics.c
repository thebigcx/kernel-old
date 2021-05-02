#include <drivers/graphics/graphics.h>

graphics_data_t graphics_data;

static uint32_t clear_color = 0x00000000;
static uint32_t text_curs_x = 0;
static uint32_t text_curs_y = 0;

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

void _putchar(char c)
{
    if (c == '\n')
    {
        text_curs_x = 0;
        text_curs_y++;
        return;
    }

    char* font = (char*)graphics_data.font->glyph_buf + (c * graphics_data.font->header->c_size);

    uint32_t x_off = text_curs_x * 8;
    uint32_t y_off = text_curs_y * 16;

    for (uint64_t y = y_off; y < y_off + 16; y++)
    {
        for (uint64_t x = x_off; x < x_off + 8; x++)
        {
            if ((*font & (0b10000000 >> (x - x_off))) > 0)
            {
                *(uint32_t*)(graphics_data.fb_adr + x * 4 + (4 * y * graphics_data.pix_per_line)) = 0xFFFFFFFF;
            }
        }
        font++;
    }

    text_curs_x++;

    if (text_curs_x > graphics_data.pix_per_line / 8)
    {
        text_curs_x = 0;
        text_curs_y++;
    }
}