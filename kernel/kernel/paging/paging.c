#include <paging/paging.h>
#include <string.h>
#include <bitmap.h>
#include <gdt/idt.h>
#include <stdio.h>
#include <system.h>

// Definitions
static pml4_t* kpml4;

uint8_t* temp_mem;

uint64_t free_memory;
uint64_t used_memory;
uint64_t reserved_memory;

bitmap_t map;

void set_page_frame(uint64_t* page, uint64_t addr)
{
    *page = (*page & ~PAGE_FRAME) | (addr & PAGE_FRAME);
}

void* temp_kmalloc(size_t sz)
{
    void* ret = temp_mem;
    temp_mem = (uint8_t*)((size_t)temp_mem + sz);

    return ret;
}

void paging_init(efi_memory_descriptor* mem, uint64_t map_size, uint64_t desc_size)
{
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
    kpml4 = (pml4_t*)temp_kmalloc(PAGE_SIZE);
    memset(kpml4, 0, PAGE_SIZE);

    page_reserve_m(0, memory_size / PAGE_SIZE + 1);

    for (uint64_t i = 0; i < entries; i++)
    {
        efi_memory_descriptor* dsc = (efi_memory_descriptor*)((uint64_t)mem + (i * desc_size));
        if (dsc->type == CONVENTIONAL_MEMORY)
        {
            page_release_m(dsc->phys_adr, dsc->num_pages);
        }
    }

    page_reserve_m(0, 0x100);
    page_alloc_m(map.buffer, map.size / PAGE_SIZE + 1);
}

// Page map indexing function implementations

void* get_physaddr(void* virt_adr, pml4_t* pml4)
{
    uint32_t pml4_index = PML4_IDX(virt_adr);
    uint32_t pdp_index = PDP_IDX(virt_adr);
    uint32_t pd_index = PD_IDX(virt_adr);
    uint32_t pt_index = PT_IDX(virt_adr);

    pdp_t* pdp = (pdp_t*)((uint64_t)pml4->entries[pml4_index] & PML4_FRAME);
    page_dir_t* pd = (page_dir_t*)((uint64_t)pdp->entries[pdp_index] & PDP_FRAME);
    page_table_t* pt = (page_table_t*)((uint64_t)pd->entries[pd_index] & PD_FRAME);

    return (void*)(pt->entries[pt_index] & PAGE_FRAME);
}

void* get_kernel_physaddr(void* virt_adr)
{
    return get_physaddr(virt_adr, kpml4);
}

void page_map_memory(void* virt_adr, void* phys_adr, pml4_t* pml4)
{
    uint32_t pml4_index = PML4_IDX(virt_adr);
    uint32_t pdp_index = PDP_IDX(virt_adr);
    uint32_t pd_index = PD_IDX(virt_adr);
    uint32_t pt_index = PT_IDX(virt_adr);

    pdp_t* pdp;
    if (!pml4->entries[pml4_index] & PML4_PRESENT)
    {
        pdp = (pdp_t*)page_request();
        memset(pdp, 0, PAGE_SIZE);
        pml4->entries[pml4_index] |= PML4_PRESENT | PML4_WRITABLE;
        set_page_frame(&pml4->entries[pml4_index], (uint64_t)pdp);
    }
    else
    {
        pdp = (pdp_t*)((uint64_t)pml4->entries[pml4_index] & PML4_FRAME);
    }

    page_dir_t* dir;
    if (!pdp->entries[pdp_index] & PDP_PRESENT)
    {
        dir = (page_dir_t*)page_request();
        memset(dir, 0, PAGE_SIZE);
        pdp->entries[pdp_index] |= PDP_PRESENT | PDP_WRITABLE;
        set_page_frame(&pdp->entries[pdp_index], (uint64_t)dir);
    }
    else
    {
        dir = (page_dir_t*)((uint64_t)pdp->entries[pdp_index] & PDP_FRAME);
    }

    page_table_t* table;
    if (!dir->entries[pd_index] & PD_PRESENT)
    {
        table = (page_table_t*)page_request();
        memset(table, 0, PAGE_SIZE);
        dir->entries[pd_index] |= PD_PRESENT | PD_WRITABLE;
        set_page_frame(&dir->entries[pd_index], (uint64_t)table);
    }
    else
    {
        table = (page_table_t*)((uint64_t)dir->entries[pd_index] & PD_FRAME);
    }

    table->entries[pt_index] |= PAGE_PRESENT | PAGE_WRITABLE;
    set_page_frame(&table->entries[pt_index], (uint64_t)phys_adr);
}

void page_kernel_map_memory(void* virt_adr, void* phys_adr)
{
    page_map_memory(virt_adr, phys_adr, kpml4);
}

void page_alloc(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE;

    bitmap_set(&map, idx, true);
    free_memory -= PAGE_SIZE;
    used_memory += PAGE_SIZE;
}

void page_free(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE;

    bitmap_set(&map, idx, false);
    free_memory += PAGE_SIZE;
    used_memory -= PAGE_SIZE;
}

void page_reserve(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE;

    bitmap_set(&map, idx, true);
    reserved_memory += PAGE_SIZE;
    free_memory -= PAGE_SIZE;
}

void page_release(void* addr)
{
    uint64_t idx = (uint64_t)addr / PAGE_SIZE;

    bitmap_set(&map, idx, false);
    reserved_memory -= PAGE_SIZE;
    free_memory += PAGE_SIZE;
}

void page_alloc_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_alloc((void*)((uint64_t)addr + (i * PAGE_SIZE)));
}

void page_free_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_free((void*)((uint64_t)addr + (i * PAGE_SIZE)));
}

void page_reserve_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_reserve((void*)((uint64_t)addr + (i * PAGE_SIZE)));
}

void page_release_m(void* addr, uint64_t cnt)
{
    for (uint64_t i = 0; i < cnt; i++)
        page_release((void*)((uint64_t)addr + (i * PAGE_SIZE)));
}

void* page_request()
{
    for (size_t i = 0; i < map.size * 8; i++)
    {
        if (bitmap_get(&map, i)) continue; // Already in use
        page_alloc((void*)(i * PAGE_SIZE));
        return (void*)(i * PAGE_SIZE);
    }

    // TODO: implement swapping to file, similar to Linux swapfile / Windows pagefile
    return NULL;
}

pml4_t* page_get_kpml4()
{
    return kpml4;
}

pml4_t* page_mk_map()
{
    pml4_t* pml4 = temp_kmalloc(PAGE_SIZE);
    memset(pml4, 0, PAGE_SIZE);

    return pml4;
}