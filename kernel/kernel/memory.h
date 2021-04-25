#pragma once

#include "stddef.h"

typedef struct
{
    void* addresses[20];
    size_t address_cnt;
} memory_map_t;

extern memory_map_t memory_map;