// Framebuffer driver for GOP

#pragma once

#include <stdint.h>

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

typedef struct gl_surface
{
    uint32_t width, height;
    uint32_t* buffer;

} gl_surface_t;

typedef struct
{
    gl_surface_t fb_surf;
    gl_surface_t back_buffer;
    psf1_font* font;
} graphics_data_t;

typedef struct gl_texture
{
    gl_surface_t surface;

} gl_texture_t;

extern graphics_data_t graphics_data;

// surface.c
void gl_surface_clear(gl_surface_t* surface, uint8_t r, uint8_t g, uint8_t b);
void gl_surface_copy(gl_surface_t* dst, const gl_surface_t* src);

// texture.c
void gl_texture_create(gl_texture_t* texture);
void gl_texture_blit(gl_texture_t* texture, gl_surface_t* surf);

// gl.c
void gl_swap_buffers();

void _putchar(char c);