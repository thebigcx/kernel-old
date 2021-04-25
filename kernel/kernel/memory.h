#pragma once

#include "stdint.h"
#include "efi_mem.h"

uint64_t get_memory_size(efi_memory_descriptor* mem, uint64_t map_entries);