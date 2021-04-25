#pragma once

#include "stdint.h"
#include "efi_mem.h"

void page_alloc_init(efi_memory_descriptor* mem, uint64_t map_size, uint64_t desc_size);