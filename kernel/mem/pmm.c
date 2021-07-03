#include <mem/pmm.h>
#include <mem/paging.h>

#define BUFFER_SIZE 2097952
//#define BUFFER_SIZE PHYS_MEM_SIZE / PAGE_SIZE_4K / 8 + 1

uint64_t bitmapsize;
uint8_t mapbuffer[BUFFER_SIZE];
//uint8_t mapbuffer[10000000];
//uint8_t* mapbuffer = 0;

void pmm_init(uint64_t memsz)
{
    bitmapsize = BUFFER_SIZE;
    memset(mapbuffer, 0, bitmapsize);

    pmm_reserve_m(0, BUFFER_SIZE / 8);
    //pmm_alloc_m(map.buffer, map.size / PAGE_SIZE_4K + 1);
}

void pmm_alloc(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE_4K;
    bitmap_set(idx, true);
}

void pmm_free(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE_4K;
    bitmap_set(idx, false);
}

void pmm_reserve(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE_4K;
    bitmap_set(idx, true);
}

void pmm_release(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE_4K;
    bitmap_set(idx, false);
}

void pmm_alloc_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        pmm_alloc((void*)((uint64_t)addr + (i * PAGE_SIZE_4K)));
}

void pmm_free_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        pmm_free((void*)((uint64_t)addr + (i * PAGE_SIZE_4K)));
}

void pmm_reserve_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        pmm_reserve((void*)((uint64_t)addr + (i * PAGE_SIZE_4K)));
}

void pmm_release_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        pmm_release((void*)((uint64_t)addr + (i * PAGE_SIZE_4K)));
}

void* pmm_request()
{
    for (size_t i = 0; i < bitmapsize * 8; i++)
    {
        if (bitmap_get(i)) continue; // Already in use
        pmm_alloc((void*)(i * PAGE_SIZE_4K));
        return (void*)(i * PAGE_SIZE_4K);
    }

    // TODO: implement swapping to file, similar to Linux swapfile / Windows pagefile
    return NULL;
}

bool bitmap_get(uint64_t idx)
{
    uint64_t bytes = idx / 8;
    uint8_t bits = 0b10000000 >> (idx % 8);

    return mapbuffer[bytes] & bits;
}

void bitmap_set(uint64_t idx, bool v)
{
    uint64_t bytes = idx / 8;
    uint8_t bits = 0b10000000 >> (idx % 8);

    mapbuffer[bytes] &= ~bits;

    if (v)
    {
        mapbuffer[bytes] |= bits;
    }
}