#include <mem/paging.h>
#include <util/stdlib.h>
#include <util/bitmap.h>
#include <intr/idt.h>
#include <sys/system.h>
#include <mem/pmm.h>

extern void* _kernel_start;
extern void* _kernel_end;

// Definitions
static pml4_t* kpml4;
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

void paging_init()
{
    // Page-aligned
    //temp_mem = (uint8_t*)((uint64_t)temp_mem + (PAGE_SIZE - ((uint64_t)temp_mem % PAGE_SIZE)));
    //kpml4 = (pml4_t*)temp_kmalloc(PAGE_SIZE);
    asm ("mov %%cr3, %0" : "=r"(kpml4));
    /*kpml4 = pmm_request();
    memset(kpml4, 0, PAGE_SIZE);

    uint64_t vaddr = KERNEL_VIRTUAL_ADDR;
    for (uint64_t i = &_kernel_start; i < &_kernel_end; i += PAGE_SIZE)
    {
        void* addr = pmm_request();
        page_kernel_map_memory(vaddr, addr);
        vaddr += PAGE_SIZE;
    }

    asm volatile ("mov %0, %%cr3"::"r"(kpml4));*/
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
        pdp = (pdp_t*)pmm_request();
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
        dir = (page_dir_t*)pmm_request();
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
        table = (page_table_t*)pmm_request();
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

pml4_t* page_get_kpml4()
{
    return kpml4;
}

pml4_t* page_mk_map()
{
    pml4_t* pml4 = pmm_request(PAGE_SIZE);
    memset(pml4, 0, PAGE_SIZE);

    return pml4;
}

pml4_t* page_clone_pml4(pml4_t* src)
{
    return src;
    pml4_t* dst = pmm_request(); // Physical
    memset(dst, 0, PAGE_SIZE);

    for (uint32_t i = 0; i < PDPS_PER_PML4; i++)
    {
        if (src->entries[i] & PML4_PRESENT)
        {
            pdp_t* pdp_src = src->entries[i] & PML4_FRAME;

            pdp_t* pdp = pmm_request();
            memset(pdp, 0, PAGE_SIZE);
            dst->entries[i] = src->entries[i] & 0x7;
            set_page_frame(&dst->entries[i], pdp);

            for (uint32_t j = 0; j < DIRS_PER_PDP; j++)
            {
                if (pdp_src->entries[j] & PDP_PRESENT)
                {
                    page_dir_t* dir_src = pdp_src->entries[j] & PDP_FRAME;

                    page_dir_t* dir = pmm_request();
                    memset(dir, 0, PAGE_SIZE);
                    pdp->entries[j] = pdp_src->entries[j] & 0x7;
                    set_page_frame(&pdp->entries[j], dir);

                    for (uint32_t k = 0; k < TABLES_PER_DIR; k++)
                    {
                        if (dir_src->entries[k] & PD_PRESENT)
                        {
                            page_table_t* tbl_src = dir_src->entries[k] & PD_FRAME;

                            page_table_t* tbl = pmm_request();
                            memset(tbl, 0, PAGE_SIZE);
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
                                        memcpy(page, page_src, PAGE_SIZE);

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
void* page_map_mmio(void* physaddr, size_t size)
{
    void* out = pmm_request();
    page_kernel_map_memory(out, physaddr);
    return out;
}