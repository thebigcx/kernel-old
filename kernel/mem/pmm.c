#include <mem/pmm.h>
#include <mem/paging.h>
#include <util/bitmap.h>

bitmap_t map;

void pmm_init(uint64_t memsz)
{
    map.size = memsz / PAGE_SIZE / 8 + 1;
    map.buffer = (uint8_t*)temp_kmalloc(map.size);
    memset(map.buffer, 0, map.size);

    pmm_reserve_m(0, memsz / PAGE_SIZE + 1);
    //pmm_alloc_m(map.buffer, map.size / PAGE_SIZE + 1);
}

void pmm_alloc(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE;
    bitmap_set(&map, idx, true);
}

void pmm_free(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE;
    bitmap_set(&map, idx, false);
}

void pmm_reserve(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE;
    bitmap_set(&map, idx, true);
}

void pmm_release(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE;
    bitmap_set(&map, idx, false);
}

void pmm_alloc_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        pmm_alloc((void*)((uint64_t)addr + (i * PAGE_SIZE)));
}

void pmm_free_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        pmm_free((void*)((uint64_t)addr + (i * PAGE_SIZE)));
}

void pmm_reserve_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        pmm_reserve((void*)((uint64_t)addr + (i * PAGE_SIZE)));
}

void pmm_release_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        pmm_release((void*)((uint64_t)addr + (i * PAGE_SIZE)));
}

void* pmm_request()
{
    for (size_t i = 0; i < map.size * 8; i++)
    {
        if (bitmap_get(&map, i)) continue; // Already in use
        pmm_alloc((void*)(i * PAGE_SIZE));
        return (void*)(i * PAGE_SIZE);
    }

    // TODO: implement swapping to file, similar to Linux swapfile / Windows pagefile
    return NULL;
}