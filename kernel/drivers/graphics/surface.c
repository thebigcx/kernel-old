#include <drivers/graphics/graphics.h>
#include <string.h>

graphics_data_t graphics_data;

static uint32_t text_curs_x = 0;
static uint32_t text_curs_y = 0;

void gl_surface_clear(gl_surface_t* surface, uint8_t r, uint8_t g, uint8_t b)
{
    uint8_t br = (uint8_t)(r * 255.0f);
    uint8_t bg = (uint8_t)(g * 255.0f);
    uint8_t bb = (uint8_t)(b * 255.0f);
    uint8_t ba = 1;

    uint32_t color = (ba << 24) |
                     (br << 16) |
                     (bg << 8) | bb;

    for (uint32_t y = 0; y < surface->height; y++)
    for (uint32_t x = 0; x < surface->width; x++)
    {
        *((uint32_t*)(surface->buffer + surface->width * y + x)) = color;
    }
}

void gl_surface_copy(gl_surface_t* dst, const gl_surface_t* src)
{
    memcpy(dst->buffer, src->buffer, dst->width * dst->height);
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
                *(uint32_t*)(graphics_data.fb_surf.buffer + x + (y * graphics_data.fb_surf.width)) = 0xFFFFFFFF;
            }
        }
        font++;
    }

    text_curs_x++;

    if (text_curs_x > graphics_data.fb_surf.width / 8)
    {
        text_curs_x = 0;
        text_curs_y++;
    }
}