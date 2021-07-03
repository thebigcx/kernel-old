#include <mem/paging.h>
#include <util/stdlib.h>
#include <intr/idt.h>
#include <sys/system.h>
#include <mem/pmm.h>
#include <mem/kheap.h>

extern void* _kernel_start;
extern void* _kernel_end;

#define KHEAP_PDP_IDX  511
#define KHEAP_PML4_IDX 511

// Definitions
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

    uint64_t kpml4_phys = (uint64_t)&kpml4 - KERNEL_VIRTUAL_ADDR;
    asm ("mov %0, %%cr3" :: "r"(kpml4_phys));
}

// Page map indexing function implementations

void* get_physaddr(void* virt_adr, pml4_t* pml4)
{
    uint32_t pml4_index = PML4_IDX(virt_adr);
    uint32_t pdp_index = PDP_IDX(virt_adr);
    uint32_t pd_index = PD_IDX(virt_adr);
    uint32_t pt_index = PT_IDX(virt_adr);

    if (!(pml4->entries[pml4_index] & PML4_PRESENT))
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

    return (void*)(pt->entries[pt_index] & PAGE_FRAME);
}

void* get_kernel_physaddr(void* virt_adr)
{
    return get_physaddr(virt_adr, &kpml4);
}

// TODO: use the cnt parameter and refactor
void page_map_memory(void* virt_adr, void* phys_adr, pml4_t* pml4, uint32_t cnt)
{
    uint32_t pml4_index = PML4_IDX(virt_adr);
    uint32_t pdp_index = PDP_IDX(virt_adr);
    uint32_t pd_index = PD_IDX(virt_adr);
    uint32_t pt_index = PT_IDX(virt_adr);

    pdp_t* pdp;
    if (!pml4->entries[pml4_index] & PML4_PRESENT)
    {
        pdp = (pdp_t*)pmm_request();
        memset(pdp, 0, PAGE_SIZE_4K);
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
        dir = (page_dir_t*)pmm_request();
        memset(dir, 0, PAGE_SIZE_4K);
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
        table = (page_table_t*)pmm_request();
        memset(table, 0, PAGE_SIZE_4K);
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

pml4_t* page_get_kpml4()
{
    return &kpml4;
}

pml4_t* page_mk_map()
{
    pml4_t* pml4 = pmm_request(PAGE_SIZE_4K);
    memset(pml4, 0, PAGE_SIZE_4K);

    return pml4;
}

pml4_t* page_clone_pml4(pml4_t* src)
{
    return src;
    pml4_t* dst = pmm_request(); // Physical
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
    }
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