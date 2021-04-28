#pragma once

#include "stdint.h"
#include "stdbool.h"

#define PAGE_SIZE 4096
#define PAGE_ENTRY_CNT 1024

typedef struct
{
    bool present : 1;
    bool read_write : 1;
    bool user_super : 1;
    bool write_thru : 1;
    bool cache_disabled : 1;
    bool accessed : 1;
    bool ign0 : 1; // Ignore bit
    bool page_size : 1;
    bool ign1 : 1; // Ignore bit
    uint8_t avail : 3;
    uint64_t address : 20;
} page_dir_entry_t;

typedef struct
{
    bool present : 1;
    bool read_write : 1;
    bool user_super : 1;
    bool write_thru : 1;
    bool cache_disabled : 1;
    bool accessed : 1;
    bool dirty : 1;
    bool ign0 : 1; // Ignore bit
    bool global : 1;
    uint8_t avail : 3;
    uint64_t address : 20;
} page_table_entry_t;

extern page_dir_entry_t page_directory[PAGE_ENTRY_CNT] __attribute__((aligned(PAGE_SIZE))); // 4KiB aligned
extern page_table_entry_t page_table[PAGE_ENTRY_CNT] __attribute__((aligned(PAGE_SIZE)));

void page_init();
void* get_physaddr(void* virt_adr);
void map_memory(void* virt_adr, void* phys_adr);