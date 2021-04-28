// Page map indexing function implementations

#include "paging.h"
#include "page_alloc.h"

void* get_physaddr(void* virt_adr)
{
    uint64_t pdindex = (uint64_t)virt_adr >> 22; // Directory
    uint64_t ptindex = (uint64_t)virt_adr >> 12 & 0x03FF; // Table

    uint64_t* pd = (uint64_t*)0xFFFFF000;

    uint64_t* pt = ((uint64_t*)0xFFC00000) + (0x400 * pdindex);

    return (void*)((pt[ptindex] & ~0xFFF) + (uint64_t)virt_adr & 0xFFF);
}

void map_memory(void* virt_adr, void* phys_adr)
{
    uint64_t pdindex = (uint64_t)virt_adr >> 22; // Directory
    uint64_t ptindex = (uint64_t)virt_adr >> 12 & 0x03FF; // Table

    page_dir_entry_t* pd = page_directory;
    page_table_entry_t* pt;
    if (!pd[pdindex].present)
    {
        pt = page_request();
        pd[pdindex].address = (uint64_t)pt >> 12;
        pd[pdindex].present = true;
    }
    else
    {
        pt = pd[pdindex].address << 12;
    }
    
    if (!pt[ptindex].present)
    {
        void* page = page_request();
        pt[ptindex].address = (uint64_t)page >> 12;
        pt[ptindex].present = true;
    }

    pt[ptindex].address = (uint64_t)phys_adr >> 12;
}