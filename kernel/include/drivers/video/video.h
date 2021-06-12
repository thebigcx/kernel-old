// Framebuffer driver
#pragma once

#include <stdint.h>

typedef struct vid_mode
{
    uint32_t width;
    uint32_t height;
    uint16_t depth; // Bits per pixel

    void* fb;

} vid_mode_t;

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

void video_init(vid_mode_t mode);
void video_set_fnt(psf1_font* fnt);
const vid_mode_t* video_get_mode();

void video_putchar(char c, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void video_puts(const char* str, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void video_putpix(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);