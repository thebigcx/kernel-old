#include "page_alloc.h"
#include "stdbool.h"
#include "../bitmap.h"
#include "../memory/memory.h"

uint64_t free_memory;
uint64_t used_memory;
uint64_t reserved_memory;
bitmap_t map;
bool init = false;

void page_alloc_init(efi_memory_descriptor* mem, uint64_t map_size, uint64_t desc_size)
{
    if (init) return; // Only need to run once
    init = true;

    uint64_t entries = map_size / desc_size;

    void* largest_seg = NULL;
    size_t largest_seg_size = 0;

    for (uint64_t i = 0; i < entries; i++)
    {
        efi_memory_descriptor* dsc = (efi_memory_descriptor*)((uint64_t)mem + (i * desc_size));
        if (dsc->type == CONVENTIONAL_MEMORY)
        {
            uint64_t size = dsc->num_pages * PAGE_SIZE;
            if (size > largest_seg_size)
            {
                largest_seg = dsc->phys_adr;
                largest_seg_size = size;
            }
        }
    }

    uint64_t memory_size = get_memory_size(mem, entries, desc_size);
    free_memory = memory_size;
    uint64_t buf_size = memory_size / PAGE_SIZE / 8 + 1;

    // Initialize bitmap

    map.size = buf_size;
    map.buffer = (uint8_t*)largest_seg;
    for (uint64_t i = 0; i < map.size; i++)
    {
        map.buffer[i] = 0;
    }

    page_alloc_m(map.buffer, map.size / PAGE_SIZE + 1);

    for (uint64_t i = 0; i < entries; i++)
    {
        efi_memory_descriptor* dsc = (efi_memory_descriptor*)((uint64_t)mem + (i * desc_size));
        if (dsc->type != CONVENTIONAL_MEMORY)
        {
            page_reserve_m(dsc->phys_adr, dsc->num_pages);
        }
    }
}

// Allocate a page at address
void page_alloc(void* adr)
{
    uint64_t idx = (uint64_t)adr / PAGE_SIZE;

    bitmap_set(&map, idx, true);
    free_memory -= PAGE_SIZE;
    used_memory += PAGE_SIZE;
}

// Free a page at address
void page_free(void* adr)
{
    uint64_t idx = (uint64_t)adr / PAGE_SIZE;

    bitmap_set(&map, idx, false);
    free_memory += PAGE_SIZE;
    used_memory -= PAGE_SIZE;
}

void page_reserve(void* adr)
{
    uint64_t idx = (uint64_t)adr / PAGE_SIZE;

    bitmap_set(&map, idx, true);
    reserved_memory += PAGE_SIZE;
    free_memory -= PAGE_SIZE;
}

// Release reserved page
void page_release(void* adr)
{
    uint64_t idx = (uint64_t)adr / PAGE_SIZE;

    bitmap_set(&map, idx, false);
    reserved_memory -= PAGE_SIZE;
    free_memory += PAGE_SIZE;
}

void page_alloc_m(void* adr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_alloc((void*)((uint64_t)adr + (i * PAGE_SIZE)));
}

void page_free_m(void* adr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_free((void*)((uint64_t)adr + (i * PAGE_SIZE)));
}

void page_reserve_m(void* adr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_reserve((void*)((uint64_t)adr + (i * PAGE_SIZE)));
}

void page_release_m(void* adr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_release((void*)((uint64_t)adr + (i * PAGE_SIZE)));
}

void* page_request()
{
    for (size_t i = 0; i < map.size; i++)
    {
        if (map.buffer[i]) continue; // Already in use
        page_alloc((void*)(i * PAGE_SIZE));
        return (void*)(i * PAGE_SIZE);
    }

    // TODO: implement swapping to file, similar to Linux swap partition
    return NULL;
}