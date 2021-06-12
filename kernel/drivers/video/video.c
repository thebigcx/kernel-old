#include <drivers/video/video.h>
#include <stddef.h>
#include <string.h>

vid_mode_t vidmode;
psf1_font* font;

static uint32_t rgbtopix(uint8_t r, uint8_t g, uint8_t b)
{
    return r << 16 | g << 8 | b;
}

void video_init(vid_mode_t mode)
{
    vidmode = mode;
}

void video_set_fnt(psf1_font* fnt)
{
    font = fnt;
}

const vid_mode_t* video_get_mode()
{
    return &vidmode;
}

void video_putchar(char c, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    char* face = (char*)font->glyph_buf + (c * font->header->c_size);

    for (uint64_t j = y; j < y + 16; j++)
    {
        for (uint64_t i = x; i < x + 8; i++)
        {
            if ((*face & (0b10000000 >> (i - x))) > 0)
            {
                *((uint32_t*)vidmode.fb + i + (j * vidmode.width)) = rgbtopix(r, g, b);
            }
        }
        face++;
    }
}

void video_puts(const char* str, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    uint32_t xoff = 0;
    
    while (*str != 0)
    {
        video_putchar(*str, x + xoff, y, r, g, b);
        xoff += 8;
        str++;
    }
}

void video_putpix(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b)
{
    *((uint32_t*)vidmode.fb + x + (y * vidmode.width)) = rgbtopix(r, g, b);
}