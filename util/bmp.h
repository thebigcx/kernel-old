#pragma once

#include <util/types.h>

typedef struct bmp_hdr
{
    uint8_t  magic[2];
    uint32_t size;
    uint32_t res;
    uint32_t off;

} __attribute__((packed)) bmp_hdr_t;

typedef struct bmp_inf_hdr
{
    uint32_t hdr_size;
    int32_t  width;
    int32_t  height;
    uint16_t planes;
    uint16_t bpp;
    uint32_t compress;
    uint32_t size;
    int32_t  hres;
    int32_t  vres;
    uint32_t palette;
    uint32_t imp_colors;

} __attribute__((packed)) bmp_inf_hdr_t;

typedef struct bmp_pal
{
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t res;

} __attribute__((packed)) bmp_pal_t;

uint8_t* bmp_load(void* data, int* w, int* h);