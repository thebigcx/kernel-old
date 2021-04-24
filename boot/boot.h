#pragma once

#include <stdint.h>

typedef struct
{
    uint64_t mem_map_size;
    uint64_t mem_map_key;
    uint64_t mem_map_desc_size;
    uint32_t mem_map_desc_vers;
    void* mem_map;
} boot_state_t;

extern boot_state_t boot_state;