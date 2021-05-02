#include <bitmap.h>

void bitmap_create(bitmap_t* bm)
{
    bm->size = 0;
    bm->buffer = NULL;
}

bool bitmap_get(bitmap_t* bm, uint64_t idx)
{
    uint64_t bytes = idx / 8;
    uint8_t bits = 0b10000000 >> (idx % 8);

    return bm->buffer[bytes] & bits;
}

void bitmap_set(bitmap_t* bm, uint64_t idx, bool v)
{
    uint64_t bytes = idx / 8;
    uint8_t bits = 0b10000000 >> (idx % 8);

    bm->buffer[bytes] &= ~bits;

    if (v)
    {
        bm->buffer[bytes] |= bits;
    }
}