#include "paging.h"
#include "string.h"
#include "../bitmap.h"
#include "../memory/memory.h"

// Definitions
static page_dir_t* page_dir;

uint8_t* temp_mem;

void* temp_malloc(size_t sz)
{
    void* ret = temp_mem;
    temp_mem = (size_t)temp_mem + sz;

    return ret;
}

void page_init()
{
    
}

// Page map indexing function implementations

void* get_physaddr(void* virt_adr)
{
    uint32_t pd_index = PAGEDIR_IDX(virt_adr);
    uint32_t pt_index = PAGETBL_IDX(virt_adr);
    uint32_t pf_off = PAGEFRAME_IDX(virt_adr); // Page frame offset

    page_table_t* table = (page_table_t*)((uint64_t)page_dir->tables[pd_index].frame << 12);
    uint32_t t = table->pages[pt_index].frame;

    t = (t << 12) + pf_off;
    return (void*)(uint64_t)t;
}

void map_memory(void* virt_adr, void* phys_adr)
{
    uint32_t pd_index = PAGEDIR_IDX(virt_adr);
    uint32_t pt_index = PAGETBL_IDX(virt_adr);

    page_table_t* table;// = page_dir->ref_tables[pd_index];
    if (!page_dir->tables[pd_index].present)
    {
        temp_mem = (uint64_t)temp_mem + (PAGE_SIZE - ((uint64_t)temp_mem % PAGE_SIZE));
        table = (page_table_t*)temp_malloc(PAGE_SIZE);
        memset(table, 0, PAGE_SIZE);
        page_dir->tables[pd_index].present = true;
        page_dir->tables[pd_index].rw = true;
        page_dir->tables[pd_index].frame = (uint64_t)table >> 12;
    }
    else
    {
        table = (page_table_t*)((uint64_t)page_dir->tables[pd_index].frame << 12);
    }

    table->pages[pt_index].frame = (uint64_t)phys_adr >> 12;
    table->pages[pt_index].rw = true;
    table->pages[pt_index].present = true;
}

uint64_t free_memory;
uint64_t used_memory;
uint64_t reserved_memory;
bitmap_t map;
bool init = false;

void page_alloc_init(efi_memory_descriptor* mem, uint64_t map_size, uint64_t desc_size)
{
    if (init) return; // Only need to run once
    init = true;

    uint64_t entries = map_size / desc_size;

    void* largest_seg = NULL;
    size_t largest_seg_size = 0;

    for (uint64_t i = 0; i < entries; i++)
    {
        efi_memory_descriptor* dsc = (efi_memory_descriptor*)((uint64_t)mem + (i * desc_size));
        if (dsc->type == CONVENTIONAL_MEMORY)
        {
            uint64_t size = dsc->num_pages * PAGE_SIZE;
            if (size > largest_seg_size)
            {
                largest_seg = dsc->phys_adr;
                largest_seg_size = size;
            }
        }
    }

    uint64_t memory_size = get_memory_size(mem, entries, desc_size);
    free_memory = memory_size;
    uint64_t buf_size = memory_size / PAGE_SIZE / 8 + 1;

    // Initialize bitmap

    map.size = buf_size;
    map.buffer = (uint8_t*)largest_seg;
    memset(map.buffer, 0, map.size);

    temp_mem = (uint8_t*)((uint64_t)map.buffer + map.size * 8);

    // Page-aligned
    temp_mem = (uint8_t*)((uint64_t)temp_mem + (PAGE_SIZE - ((uint64_t)temp_mem % PAGE_SIZE)));
    page_dir = (page_dir_t*)temp_malloc(PAGE_SIZE);
    //page_dir = (page_dir_t*)0x7C01000;
    memset(page_dir, 0, PAGE_SIZE);

    page_alloc_m(map.buffer, map.size / PAGE_SIZE + 1);

    for (uint64_t i = 0; i < entries; i++)
    {
        efi_memory_descriptor* dsc = (efi_memory_descriptor*)((uint64_t)mem + (i * desc_size));
        if (dsc->type != CONVENTIONAL_MEMORY)
        {
            page_reserve_m(dsc->phys_adr, dsc->num_pages);
        }
    }
}

// Allocate a page at address
void page_alloc(void* adr)
{
    uint64_t idx = (uint64_t)adr / PAGE_SIZE;

    bitmap_set(&map, idx, true);
    free_memory -= PAGE_SIZE;
    used_memory += PAGE_SIZE;
}

// Free a page at address
void page_free(void* adr)
{
    uint64_t idx = (uint64_t)adr / PAGE_SIZE;

    bitmap_set(&map, idx, false);
    free_memory += PAGE_SIZE;
    used_memory -= PAGE_SIZE;
}

void page_reserve(void* adr)
{
    uint64_t idx = (uint64_t)adr / PAGE_SIZE;

    bitmap_set(&map, idx, true);
    reserved_memory += PAGE_SIZE;
    free_memory -= PAGE_SIZE;
}

// Release reserved page
void page_release(void* adr)
{
    uint64_t idx = (uint64_t)adr / PAGE_SIZE;

    bitmap_set(&map, idx, false);
    reserved_memory -= PAGE_SIZE;
    free_memory += PAGE_SIZE;
}

void page_alloc_m(void* adr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_alloc((void*)((uint64_t)adr + (i * PAGE_SIZE)));
}

void page_free_m(void* adr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_free((void*)((uint64_t)adr + (i * PAGE_SIZE)));
}

void page_reserve_m(void* adr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_reserve((void*)((uint64_t)adr + (i * PAGE_SIZE)));
}

void page_release_m(void* adr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_release((void*)((uint64_t)adr + (i * PAGE_SIZE)));
}

void* page_request()
{
    for (size_t i = 0; i < map.size * 8; i++)
    {
        if (bitmap_get(&map, i)) continue; // Already in use
        page_alloc((void*)(i * PAGE_SIZE));
        return (void*)(i * PAGE_SIZE);
    }

    // TODO: implement swapping to file, similar to Linux swap partition
    return NULL;
}

void set_page_dir(page_dir_t* dir)
{
    page_dir = dir;
}

page_dir_t* page_dir_get()
{
    return page_dir;
}