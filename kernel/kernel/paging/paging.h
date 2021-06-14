#pragma once

#include <types.h>

#include <mem/mem.h>

// Page size and entry counts
#define PAGE_SIZE          4096
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
#define PML4_FRAME         0xFFFFFFFFFF000

#define PDP_PRESENT        1
#define PDP_WRITABLE       (1 << 1)
#define PDP_USER           (1 << 2)
#define PDP_FRAME          0xFFFFFFFFFF000

#define PD_PRESENT         1
#define PD_WRITABLE        (1 << 1)
#define PD_USER            (1 << 2)
#define PD_FRAME           0xFFFFFFFFFF000

#define PAGE_PRESENT       1
#define PAGE_WRITABLE      (1 << 1)
#define PAGE_USER          (1 << 2)
#define PAGE_WRITETHROUGH  (1 << 3)
#define PAGE_CACHEDISABLED (1 << 4)
#define PAGE_FRAME         0xFFFFFFFFFF000ULL

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
} __attribute__((aligned(PAGE_SIZE))) pml4_t;

typedef struct pdp
{
    pdp_entry_t entries[DIRS_PER_PDP];
} __attribute__((aligned(PAGE_SIZE))) pdp_t;

typedef struct page_dir
{
    pd_entry_t entries[TABLES_PER_DIR];
} __attribute__((aligned(PAGE_SIZE))) page_dir_t;

typedef struct page_table
{
    page_t entries[PAGES_PER_TABLE];
} __attribute__((aligned(PAGE_SIZE))) page_table_t;

typedef struct page_map
{
    pml4_t* pml4;
    uint64_t pml4_phys;

} __attribute__((packed)) page_map_t;

void paging_init(efi_memory_descriptor* mem, uint64_t map_size, uint64_t desc_size);
void* get_physaddr(void* virt_adr, pml4_t* pml4);
void* get_kernel_physaddr(void* virt_adr);

// Map virtual address to physical address
void page_map_memory(void* virt_adr, void* phys_adr, pml4_t* pml4);
// Map memory for kernel
void page_kernel_map_memory(void* virt_adr, void* phys_adr);
// Allocate a page at physical address
void page_alloc(void* addr);
// Free a page at physical address
void page_free(void* addr);
// Reserve a page at physical address
void page_reserve(void* addr);
// Release a reserved page at physical address
void page_release(void* addr);

// Allocate multiple pages
void page_alloc_m(void* addr, uint64_t cnt);
// Free multiple pages
void page_free_m(void* addr, uint64_t cnt);
// Reserve multiple pages
void page_reserve_m(void* addr, uint64_t cnt);
// Release multiple reserved pages
void page_release_m(void* addr, uint64_t cnt);

// Create a page map
pml4_t* page_mk_map();

void* page_request();
pml4_t* page_get_kpml4();