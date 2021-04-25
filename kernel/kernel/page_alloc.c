#include "page_alloc.h"
#include "stdbool.h"
#include "bitmap.h"
#include "memory.h"

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
        if (mem[i].type == CONVENTIONAL_MEMORY)
        {
            uint64_t size = mem[i].num_pages * 4096;
            if (size < largest_seg_size)
            {
                largest_seg = mem[i].phys_adr;
                largest_seg_size = size;
            }
        }
    }

    uint64_t memory_size = get_memory_size(mem, entries);
    free_memory = memory_size;
    uint64_t buf_size = memory_size / 4096 / 8 + 1;

    // Initialize bitmap

    map.size = buf_size;
    map.buffer = (uint8_t*)largest_seg;
    for (uint64_t i = 0; i < map.size; i++)
    {
        map.buffer[i] = 0;
    }
}