// Framebuffer driver
#pragma once

#include <util/types.h>

typedef struct vid_mode
{
    uint32_t width;
    uint32_t height;
    uint16_t depth; // Bits per pixel

    void* fb;
    uint64_t fbphys;

} vid_mode_t;

// Bitmap fonts (TEMPORARY)
typedef struct psf1_header
{
    uint8_t magic[2];
    uint8_t mode;
    uint8_t c_size; // Character size

} psf1_header_t;

typedef struct psf1_font
{
    psf1_header_t header;
    void* glyph_buf;

} psf1_font_t;

// ioctl on /dev/fb0
typedef struct fbinfo
{
    uint32_t width;
    uint32_t height;
    uint32_t bpp;

} fbinfo_t;

void video_init();
void video_setmode(vid_mode_t mode);
//void video_set_fnt(psf1_font* fnt);
const vid_mode_t* video_get_mode();

void video_putchar(char c, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void video_puts(const char* str, uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void video_putpix(uint32_t x, uint32_t y, uint8_t r, uint8_t g, uint8_t b);
void video_draw_img(uint32_t x, uint32_t y, uint32_t w, uint32_t h, void* data);