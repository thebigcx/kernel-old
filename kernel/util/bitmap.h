#pragma once

// An optimized array of bits

#include <util/types.h>

typedef struct
{
    size_t size;
    uint8_t* buffer;
} bitmap_t;

void bitmap_create(bitmap_t* bm);
bool bitmap_get(bitmap_t* bm, uint64_t idx);
void bitmap_set(bitmap_t* bm, uint64_t idx, bool v);