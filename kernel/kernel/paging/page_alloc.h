#pragma once

#include "stdint.h"
#include "../memory/efi_mem.h"
#include "paging.h"

void page_alloc_init(efi_memory_descriptor* mem, uint64_t map_size, uint64_t desc_size);

void page_alloc(void* adr);
void page_free(void* adr);
void page_reserve(void* adr);
void page_release(void* adr);

// Multiple pages
void page_alloc_m(void* adr, uint64_t cnt);
void page_free_m(void* adr, uint64_t cnt);
void page_reserve_m(void* adr, uint64_t cnt);
void page_release_m(void* adr, uint64_t cnt);

void* page_request();