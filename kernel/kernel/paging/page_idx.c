// Page map indexing function implementations

#include "paging.h"

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
    
}