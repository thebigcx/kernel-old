#pragma once

#include <util/types.h>

void pmm_init(uint64_t memsz);

// Allocate a page at physical address
void pmm_alloc(void* addr);
// Free a page at physical address
void pmm_free(void* addr);
// Reserve a page at physical address
void pmm_reserve(void* addr);
// Release a reserved page at physical address
void pmm_release(void* addr);

// Allocate multiple pages
void pmm_alloc_m(void* addr, uint64_t cnt);
// Free multiple pages
void pmm_free_m(void* addr, uint64_t cnt);
// Reserve multiple pages
void pmm_reserve_m(void* addr, uint64_t cnt);
// Release multiple reserved pages
void pmm_release_m(void* addr, uint64_t cnt);

void* pmm_request();