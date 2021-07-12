#pragma once

#include <stdint.h>

#define FB_GET_INFO 0

typedef struct fbinfo
{
    uint32_t width;
    uint32_t height;
    uint32_t bpp;

} fbinfo_t;