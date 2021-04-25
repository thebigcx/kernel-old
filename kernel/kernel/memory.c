#include "memory.h"

uint64_t get_memory_size(efi_memory_descriptor* mem, uint64_t map_entries)
{
    uint64_t size = 0;

    for (uint64_t i = 0; i < map_entries; i++)
        size += mem[i].num_pages * 4096;

    return size;
}