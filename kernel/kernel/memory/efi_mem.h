#pragma once

#include "stdint.h"

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