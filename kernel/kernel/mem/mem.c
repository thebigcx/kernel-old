#include <mem/mem.h>

uint64_t mem_sz;

void mem_boot_data(efi_memory_descriptor* mem, uint64_t map_entries, uint64_t desc_size)
{
    mem_sz = 0;

    for (uint64_t i = 0; i < map_entries; i++)
    {
        // note: sizeof(efi_memory_descriptor) != desc_size
        // have to do this instead of mem[i]
        efi_memory_descriptor* dsc = (efi_memory_descriptor*)((uint64_t)mem + (i * desc_size));
        mem_sz += dsc->num_pages * 4096;
    }
}

uint64_t mem_get_sz()
{
    return mem_sz;
}