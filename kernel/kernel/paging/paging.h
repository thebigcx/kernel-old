#pragma once

#include "stdint.h"
#include "stdbool.h"
#include "../memory/efi_mem.h"

#define PAGE_SIZE 4096
#define PAGE_ENTRY_CNT 512

#define PAGEDIRPTR_IDX(vadr) ((((uint64_t)vadr) >> 39) & 0x1ff)
#define PAGEDIR_IDX(vadr) ((((uint64_t)vadr) >> 30) & 0x1ff)
#define PAGETBL_IDX(vadr) ((((uint64_t)vadr) >> 21) & 0x1ff)
#define PAGEFRAME_IDX(vadr) ((((uint64_t)vadr) >> 12) & 0x1ff)

typedef struct page_dir_entry
{
    bool present   : 1;
    bool rw        : 1;
    bool user      : 1;
    bool w_thru    : 1;
    bool cache     : 1;
    bool access    : 1;
    bool res       : 1; // Ignore
    bool page_size : 1;
    bool global    : 1; // Ignore
    uint8_t avail  : 3;
    uint64_t frame : 52;
} page_dir_entry_t;

typedef struct page_table_entry
{
    bool present   : 1;
    bool rw        : 1;
    bool user      : 1;
    bool res       : 2; // Ignore
    bool access    : 1;
    bool dirty     : 1;
    bool res2      : 2;
    uint8_t avail  : 3;
    uint64_t frame : 52;
} page_table_entry_t;

typedef struct page_table
{
    page_table_entry_t entries[PAGE_ENTRY_CNT];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

typedef struct page_dir
{
    page_dir_entry_t entries[PAGE_ENTRY_CNT];
} __attribute__((aligned(PAGE_SIZE))) page_dir_t;

void page_init();
void* get_physaddr(void* virt_adr);
void map_memory(void* virt_adr, void* phys_adr);

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
page_dir_t* page_get_pml4();