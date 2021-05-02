#pragma once

#include <stdint.h>
#include <stddef.h>

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

uint64_t get_memory_size(efi_memory_descriptor* mem, uint64_t map_entries, uint64_t desc_size);