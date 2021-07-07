#include <mem/paging.h>
#include <util/stdlib.h>
#include <intr/idt.h>
#include <sys/system.h>
#include <mem/pmm.h>
#include <mem/kheap.h>
#include <util/assert.h>

extern void* _kernel_start;
extern void* _kernel_end;

#define KHEAP_PDP_IDX  511
#define KHEAP_PML4_IDX 511

// Definitions
uint64_t kpml4_phys;
pml4_t kpml4 __attribute__((aligned(PAGE_SIZE_4K)));
pdp_t kpdp __attribute__((aligned(PAGE_SIZE_4K)));
page_dir_t kpd __attribute__((aligned(PAGE_SIZE_4K)));

page_dir_t kheapdir __attribute__((aligned(PAGE_SIZE_4K)));
page_t kheap_tables[TABLES_PER_DIR][PAGES_PER_TABLE] __attribute__((aligned(PAGE_SIZE_4K)));

page_dir_t iodirs[4] __attribute__((aligned(PAGE_SIZE_4K)));
uint8_t* temp_mem = 0;

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

// Set up higher half mappings
void paging_init()
{
    memset(&kpml4, 0, sizeof(pml4_t));
    memset(&kpdp, 0, sizeof(pdp_t));

    set_page_frame(&(kpml4.entries[PML4_IDX(KERNEL_VIRTUAL_ADDR)]), ((uint64_t)&kpdp - KERNEL_VIRTUAL_ADDR));
    kpml4.entries[PML4_IDX(KERNEL_VIRTUAL_ADDR)] |= PML4_PRESENT | PML4_WRITABLE;
    kpml4.entries[0] = kpml4.entries[PML4_IDX(KERNEL_VIRTUAL_ADDR)];

    kpdp.entries[PDP_IDX(KERNEL_VIRTUAL_ADDR)] = ((uint64_t)&kpd - KERNEL_VIRTUAL_ADDR) | PDP_PRESENT | PDP_WRITABLE;
    for (uint32_t i = 0; i < TABLES_PER_DIR; i++)
    {
        kpd.entries[i] = (PAGE_SIZE_2M * i) | PDP_PRESENT | PDP_WRITABLE | PD_2M;
    }

    kpdp.entries[KHEAP_PDP_IDX] = PDP_PRESENT | PDP_WRITABLE;
    set_page_frame(&(kpdp.entries[KHEAP_PDP_IDX]), (uint64_t)&kheapdir - KERNEL_VIRTUAL_ADDR);

    for (uint32_t i = 0; i < 4; i++)
    {
        kpdp.entries[PDP_IDX(IO_VIRTUAL_ADDR) + i] = ((uint64_t)&iodirs[i] - KERNEL_VIRTUAL_ADDR) | PDP_PRESENT | PDP_WRITABLE;

        for (uint32_t j = 0; j < TABLES_PER_DIR; j++)
        {
            iodirs[i].entries[j] = (PAGE_SIZE_1G * i + PAGE_SIZE_2M * j) | (PD_2M | PDP_WRITABLE | PDP_PRESENT | PD_CACHEDISABLED);
        }
    }

    // Identity map low memory
    kpdp.entries[0] = kpdp.entries[PDP_IDX(KERNEL_VIRTUAL_ADDR)];

    for (uint32_t i = 0; i < TABLES_PER_DIR; i++)
    {
        memset(&(kheap_tables[i]), 0, sizeof(page_t) * PAGES_PER_TABLE);
    }

    kpml4_phys = (uint64_t)&kpml4 - KERNEL_VIRTUAL_ADDR;
    asm ("mov %0, %%cr3" :: "r"(kpml4_phys));
}

// Page map indexing function implementations

void* page_getphys(void* virt, page_map_t* map)
{
    uint32_t pml4_index = PML4_IDX(virt);
    uint32_t pdp_index = PDP_IDX(virt);
    uint32_t pd_index = PD_IDX(virt);
    uint32_t pt_index = PT_IDX(virt);

    /*if (!(pml4->entries[pml4_index] & PML4_PRESENT))
        serial_writestr("Page Directory Pointer not present!\n");

    pdp_t* pdp = (pdp_t*)((uint64_t)pml4->entries[pml4_index] & PML4_FRAME);

    if (!(pdp->entries[pdp_index] & PDP_PRESENT))
        serial_writestr("Page Directory not present!\n");

    page_dir_t* pd = (page_dir_t*)((uint64_t)pdp->entries[pdp_index] & PDP_FRAME);

    if (!(pd->entries[pd_index] & PD_PRESENT))
        serial_writestr("Page Table not present!\n");

    page_table_t* pt = (page_table_t*)((uint64_t)pd->entries[pd_index] & PD_FRAME);

    if (!(pt->entries[pt_index] & PAGE_PRESENT))
        serial_writestr("Page not present!\n");

    return (void*)(pt->entries[pt_index] & PAGE_FRAME);*/

    // User space
    if (pml4_index == 0)
    {
        if (!(map->page_dirs[pdp_index][pd_index] & PD_PRESENT) || !(map->page_tables[pdp_index][pd_index])) return 0;

        return map->page_tables[pdp_index][pd_index][pt_index] & PAGE_FRAME;
    }
    else // Kernel space
    {
        return 0;
    }
}

void* page_kernel_getphys(void* virt)
{
    //return page_getphys(virt, &kpml4);
}

// TODO: fix bug where this function fucks up in userspace
void page_map_memory(void* virt, void* phys, uint32_t cnt, page_map_t* map)
{
    while (cnt--)
    {
        uint64_t pml4idx = PML4_IDX(virt);
        uint64_t pdpidx = PDP_IDX(virt);
        uint64_t pdidx = PD_IDX(virt);
        uint64_t ptidx = PT_IDX(virt);

        if (!(map->page_dirs[pdpidx][pdidx] & PD_PRESENT)) page_mk_table(pdpidx, pdidx, map);

        set_page_frame(&(map->page_tables[pdpidx][pdidx][ptidx]), phys);
        map->page_tables[pdpidx][pdidx][ptidx] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;

        invlpg(virt);

        phys += PAGE_SIZE_4K;
        virt += PAGE_SIZE_4K;
    }
}

void page_kernel_map_memory(void* virt_adr, void* phys_adr, uint32_t cnt)
{
    while (cnt--)
    {
        uint32_t pdidx = PD_IDX(virt_adr);
        uint32_t ptidx = PT_IDX(virt_adr);

        set_page_frame(&kheap_tables[pdidx][ptidx], phys_adr);
        kheap_tables[pdidx][ptidx] |= PAGE_PRESENT | PAGE_WRITABLE;
        invlpg(virt_adr);
        virt_adr += PAGE_SIZE_4K;
        phys_adr += PAGE_SIZE_4K;
    }
}

void page_mk_table(uint64_t pdpidx, uint64_t pdidx, page_map_t* map)
{
    page_table_t* table = page_kernel_alloc4k(1);
    uint64_t table_phys = pmm_request();
    page_kernel_map_memory(table, table_phys, 1);
    memset(table, 0, PAGE_SIZE_4K);

    set_page_frame(&(map->page_dirs[pdpidx][pdidx]), table_phys);
    map->page_dirs[pdpidx][pdidx] |= PAGE_PRESENT | PAGE_WRITABLE | PAGE_USER;
    
    map->page_tables[pdpidx][pdidx] = table;
}

pml4_t* page_get_kpml4()
{
    return &kpml4;
}

page_map_t* page_mk_map()
{
    pml4_t* pml4 = page_kernel_alloc4k(1);
    uint64_t pml4_phys = pmm_request();
    page_kernel_map_memory(pml4, pml4_phys, 1);
    memcpy(pml4, &kpml4, PAGE_SIZE_4K);

    pdp_t* pdp = page_kernel_alloc4k(1);
    uint64_t pdp_phys = pmm_request();
    page_kernel_map_memory(pdp, pdp_phys, 1);
    memset(pdp, 0, PAGE_SIZE_4K);

    set_page_frame(&pml4->entries[0], pdp_phys);
    pml4->entries[0] |= PML4_PRESENT | PML4_WRITABLE | PML4_USER;

    pd_entry_t** page_dirs = page_kernel_alloc4k(1);
    page_kernel_map_memory(page_dirs, pmm_request(), 1);
    uint64_t* page_dirs_phys = page_kernel_alloc4k(1);
    page_kernel_map_memory(page_dirs_phys, pmm_request(), 1);
    page_t*** page_tables = page_kernel_alloc4k(1);
    page_kernel_map_memory(page_tables, pmm_request(), 1);

    for (uint32_t i = 0; i < DIRS_PER_PDP; i++)
    {
        page_dirs[i] = page_kernel_alloc4k(1);
        page_dirs_phys[i] = pmm_request();
        page_kernel_map_memory(page_dirs[i], page_dirs_phys[i], 1);
        memset(page_dirs[i], 0, PAGE_SIZE_4K);

        set_page_frame(&pdp->entries[i], page_dirs_phys[i]);
        pdp->entries[i] |= PDP_PRESENT | PDP_WRITABLE | PDP_USER;

        page_tables[i] = page_kernel_alloc4k(1);
        memset(page_tables[i], 0, PAGE_SIZE_4K);
    }

    page_map_t* map = kmalloc(sizeof(page_map_t));
    map->pml4 = pml4;
    map->pml4_phys = pml4_phys;
    map->pdp = pdp;
    map->pdp_phys = pdp_phys;
    map->page_dirs = page_dirs;
    map->page_dirs_phys = page_dirs_phys;
    map->page_tables = page_tables;
    map->regions = list_create();
    return map;
}

page_map_t* page_clone_map(page_map_t* src)
{
    //return src;
    /*pml4_t* dst = pmm_request(); // Physical
    memset(dst, 0, PAGE_SIZE_4K);

    for (uint32_t i = 0; i < PDPS_PER_PML4; i++)
    {
        if (src->entries[i] & PML4_PRESENT)
        {
            pdp_t* pdp_src = src->entries[i] & PML4_FRAME;

            pdp_t* pdp = pmm_request();
            memset(pdp, 0, PAGE_SIZE_4K);
            dst->entries[i] = src->entries[i] & 0x7;
            set_page_frame(&dst->entries[i], pdp);

            for (uint32_t j = 0; j < DIRS_PER_PDP; j++)
            {
                if (pdp_src->entries[j] & PDP_PRESENT)
                {
                    page_dir_t* dir_src = pdp_src->entries[j] & PDP_FRAME;

                    page_dir_t* dir = pmm_request();
                    memset(dir, 0, PAGE_SIZE_4K);
                    pdp->entries[j] = pdp_src->entries[j] & 0x7;
                    set_page_frame(&pdp->entries[j], dir);

                    for (uint32_t k = 0; k < TABLES_PER_DIR; k++)
                    {
                        if (dir_src->entries[k] & PD_PRESENT)
                        {
                            page_table_t* tbl_src = dir_src->entries[k] & PD_FRAME;

                            page_table_t* tbl = pmm_request();
                            memset(tbl, 0, PAGE_SIZE_4K);
                            dir->entries[k] = dir_src->entries[k] & 0x7;
                            set_page_frame(&dir->entries[k], tbl);

                            // Copy the pages
                            for (uint32_t l = 0; l < PAGES_PER_TABLE; l++)
                            {
                                if (tbl_src->entries[l] & PAGE_PRESENT)
                                {
                                    if (tbl_src->entries[l] & PAGE_USER)
                                    {
                                        void* page_src = tbl_src->entries[l] & PAGE_FRAME;
                                        void* page = pmm_request();
                                        memcpy(page, page_src, PAGE_SIZE_4K);

                                        tbl->entries[l] = tbl_src->entries[l] & 0x1f;
                                        
                                        set_page_frame(&tbl->entries[l], page);
                                    }
                                    else // If it is a kernel page, link pages rather than copy them
                                    {
                                        tbl->entries[l] = tbl_src->entries[l];
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }*/
}

// TODO: acknowledge size parameter
void* page_map_mmio(void* physaddr)
{
    return physaddr + IO_VIRTUAL_ADDR;
}

// Contiguous
void* page_kernel_alloc4k(uint32_t cnt)
{
    uint32_t counter = 0;
    uint64_t offset = 0;
    uint64_t pdoff = 0;

    // Attempt to use pre-allocated structures
    for (uint32_t i = 0; i < TABLES_PER_DIR; i++)
    {
        if (!(kheapdir.entries[i] & PD_PRESENT) || kheapdir.entries[i] & PD_2M)
        {
            pdoff = i + 1;
            offset = 0;
            counter = 0;
            continue;
        }

        for (uint32_t j = 0; j < PAGES_PER_TABLE; j++)
        {
            if (kheap_tables[i][j] & PAGE_PRESENT)
            {
                pdoff = i;
                offset = j + 1;
                counter = 0;
                continue;
            }

            counter++;

            if (counter >= cnt)
            {
                uint64_t addr = (PDP_SIZE * KHEAP_PML4_IDX) + (KHEAP_PDP_IDX * PAGE_SIZE_1G) + (pdoff * PAGE_SIZE_2M) + (offset * PAGE_SIZE_4K);
                addr |= 0xffff000000000000;

                // Allocate the pages
                while (counter--)
                {
                    if (offset >= 512)
                    {
                        pdoff++;
                        offset = 0;
                    }

                    kheap_tables[pdoff][offset] = PAGE_PRESENT | PAGE_WRITABLE;
                    offset++;
                }

                return addr;
            }
        }
    }

    counter = 0;
    pdoff = 0;
    offset = 0;

    // Need to allocate more paging structures

    for (uint32_t i = 0; i < TABLES_PER_DIR; i++)
    {
        if (kheapdir.entries[i] & PD_PRESENT)
        {
            pdoff = i + 1;
            offset = 0;
            counter = 0;
            continue;
        }

        counter += 512;
        if (counter >= cnt)
        {
            uint64_t addr = (PDP_SIZE * KHEAP_PML4_IDX) + (KHEAP_PDP_IDX * PAGE_SIZE_1G) + (pdoff * PAGE_SIZE_2M) + (offset * PAGE_SIZE_4K);
            addr |= 0xffff000000000000;

            set_page_frame(&kheapdir.entries[pdoff], ((uint64_t)&kheap_tables[pdoff] - KERNEL_VIRTUAL_ADDR));
            kheapdir.entries[pdoff] |= PD_PRESENT | PD_WRITABLE;

            // Allocate the pages
            while (cnt--)
            {
                if (offset >= 512)
                {
                    pdoff++;
                    offset = 0;
                    set_page_frame(&kheapdir.entries[pdoff], ((uint64_t)&kheap_tables[pdoff] - KERNEL_VIRTUAL_ADDR));
                    kheapdir.entries[pdoff] |= PD_PRESENT | PD_WRITABLE;
                }

                kheap_tables[pdoff][offset] = PAGE_PRESENT | PAGE_WRITABLE;
                offset++;
            }

            return addr;
        }
    }

    serial_writestr("Could not allocate virtual memory\n");
}

void page_kernel_free4k(void* addr, uint32_t cnt)
{
    uint64_t virt = (uint64_t)addr;
    
    while (cnt--)
    {
        kheap_tables[PD_IDX(virt)][PT_IDX(virt)] = 0; // Non-present
        invlpg(virt); // Flush TLB
        virt += PAGE_SIZE_4K;
    }
}

void space_alloc_region_at(uint64_t base, uint64_t size, page_map_t* map)
{
    /*assert(base % PAGE_SIZE_4K == 0);

    uint32_t pgcnt = size / PAGE_SIZE_4K + 1;

    for (uint32_t i = 0; i < pgcnt; i++)
    {
        page_map_memory(base + i * PAGE_SIZE_4K, pmm_request(), 1, map);
    }*/

    mregion_t* newreg = kmalloc(sizeof(mregion_t));
    newreg->base = base;
    newreg->end = base + size;

    uint32_t i = 0;
    list_foreach(map->regions, node)
    {
        mregion_t* region = node->val;

        if (region->base >= base + size)
        {
            list_insert(map->regions, newreg, i);
            return newreg->base;
        }

        i++;
    }

    list_push_back(map->regions, newreg);
    return NULL;
}

uint64_t space_alloc_region(uint64_t size, page_map_t* map)
{
    uint64_t base = PAGE_SIZE_4K;
    uint64_t end = base + size;

    uint32_t i = 0;
    list_foreach(map->regions, node)
    {
        mregion_t* region = node->val;

        if (region->base >= end)
        {
            mregion_t* newreg = kmalloc(sizeof(mregion_t));
            newreg->base = base;
            newreg->end = end;

            list_insert(map->regions, newreg, i);

            return newreg->base;
        }

        // Intersects
        if (base >= region->base && base < region->end)
        {
            base = region->end;
            end = base + size;
        }

        if (end > region->base && end <= region->end)
        {
            base = region->end;
            end = base + size;
        }

        // Encapsulates
        if (base < region->base && end > region->end)
        {
            base = region->end;
            end = base + size;
        }

        i++;
    }

    if (end < KERNEL_VIRTUAL_ADDR)
    {
        mregion_t* newreg = kmalloc(sizeof(mregion_t));
        newreg->base = base;
        newreg->end = end;

        list_push_back(map->regions, newreg);
        return newreg->base;
    }

    // Could not allocate
    return NULL;
}