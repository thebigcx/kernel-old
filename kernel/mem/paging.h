#pragma once

#include <util/types.h>
#include <util/list.h>

#define KERNEL_VIRTUAL_ADDR 0xffffffff80000000ull
#define IO_VIRTUAL_ADDR     (KERNEL_VIRTUAL_ADDR - 0x100000000ull)

// Page size and entry counts
#define PAGE_SIZE_4K          4096
#define PAGE_SIZE_2M       0x200000
#define PAGE_SIZE_1G       0x40000000ull
#define PAGES_PER_TABLE    512
#define TABLES_PER_DIR     512
#define DIRS_PER_PDP       512
#define PDPS_PER_PML4      512

// Indices of page structures
#define PML4_IDX(vadr)     ((((uint64_t)vadr) >> 39) & 0x1ff)
#define PDP_IDX(vadr)      ((((uint64_t)vadr) >> 30) & 0x1ff)
#define PD_IDX(vadr)       ((((uint64_t)vadr) >> 21) & 0x1ff)
#define PT_IDX(vadr)       ((((uint64_t)vadr) >> 12) & 0x1ff)

// Page structure entry flags
#define PML4_PRESENT       1
#define PML4_WRITABLE      (1 << 1)
#define PML4_USER          (1 << 2)
#define PML4_FRAME         0xffffffffff000

#define PDP_PRESENT        1
#define PDP_WRITABLE       (1 << 1)
#define PDP_USER           (1 << 2)
#define PDP_1G             (1 << 7)
#define PDP_FRAME          0xffffffffff000

#define PD_PRESENT         1
#define PD_WRITABLE        (1 << 1)
#define PD_USER            (1 << 2)
#define PD_CACHEDISABLED  (1 << 4)
#define PD_2M             (1 << 7) // 2 MiB pages
#define PD_PAT            (1 << 12)
#define PD_FRAME           0xffffffffff000

#define PAGE_PRESENT       1
#define PAGE_WRITABLE      (1 << 1)
#define PAGE_USER          (1 << 2)
#define PAGE_WRITETHROUGH  (1 << 3)
#define PAGE_CACHEDISABLED (1 << 4)
#define PAGE_PAT           (1 << 7)
#define PAGE_FRAME         0xffffffffff000ull

#define PDP_SIZE           0x8000000000

// Page structure entries
typedef uint64_t page_t;
typedef uint64_t pd_entry_t;
typedef uint64_t pdp_entry_t;
typedef uint64_t pml4_entry_t;

/*
    Page Structures:
    Page Map Level 4, Page Directory Pointer Table, Page Directory, Page Table.
*/
typedef struct pml4
{
    pml4_entry_t entries[PDPS_PER_PML4];
} __attribute__((aligned(PAGE_SIZE_4K))) pml4_t;

typedef struct pdp
{
    pdp_entry_t entries[DIRS_PER_PDP];
} __attribute__((aligned(PAGE_SIZE_4K))) pdp_t;

typedef struct page_dir
{
    pd_entry_t entries[TABLES_PER_DIR];
} __attribute__((aligned(PAGE_SIZE_4K))) page_dir_t;

typedef struct page_table
{
    page_t entries[PAGES_PER_TABLE];
} __attribute__((aligned(PAGE_SIZE_4K))) page_table_t;

typedef struct mregion
{
    uint64_t base;
    uint64_t end;

} mregion_t;

// Maximum of 512 GB, AKA one PDP
typedef struct page_map
{
    pml4_t* pml4;
    uint64_t pml4_phys;

    pdp_t* pdp;
    uint64_t pdp_phys;

    pd_entry_t** page_dirs;
    uint64_t* page_dirs_phys;

    page_t*** page_tables;

    // Allocated regions
    list_t* regions;

} __attribute__((packed)) page_map_t;

void paging_init();
void* page_getphys(void* virt, page_map_t* map);
void* page_kernel_getphys(void* virt);

// Map virtual address to physical address
void page_map_memory(void* virt, void* phys, uint32_t cnt, page_map_t* map);
// Map memory for kernel
void page_kernel_map_memory(void* virt, void* phys, uint32_t cnt);

void* page_map_mmio(void* physaddr);

// Allocate virtual memory, physical memory is managed in pmm.c
void* page_kernel_alloc4k(uint32_t cnt);
void  page_kernel_free4k(void* addr, uint32_t cnt);

void page_mk_table(uint64_t pdpidx, uint64_t pdidx, page_map_t* map);

// Create a page map
page_map_t* page_mk_map();
// Copy a page map
page_map_t* page_clone_map(page_map_t* src);
// Destroy a page map
void page_destroy_map(page_map_t* map);

pml4_t* page_get_kpml4();

void space_alloc_region_at(uint64_t base, uint64_t size, page_map_t* map);
uint64_t space_alloc_region(uint64_t size, page_map_t* map);