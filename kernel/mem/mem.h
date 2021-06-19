#pragma once

#include <util/types.h>


typedef struct
{
    uint32_t type;
    uint32_t pad;
    void* phys_adr;
    void* virt_adr;
    uint64_t num_pages;
    uint64_t attr;
} efi_memory_descriptor;

enum
{
    CONVENTIONAL_MEMORY = 7
};

void mem_boot_data(efi_memory_descriptor* mem, uint64_t map_entries, uint64_t desc_size);
uint64_t mem_get_sz();