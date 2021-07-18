#include <util/elf.h>

int elf_load(void* data, proc_t* proc, elf_inf_t* info)
{
    elf64_hdr_t hdr;

    memcpy(&hdr, data, sizeof(elf64_hdr_t));

    if (!check_elf_hdr(&hdr))
        return 1;

    for (uint16_t i = 0; i < hdr.ph_num; i++)
    {
        elf64_phdr_t phdr;
        memcpy(&phdr, data + hdr.ph_off + hdr.ph_ent_size * i, sizeof(elf64_phdr_t));

        if (phdr.type == PT_LOAD)
        {
            uint64_t begin = phdr.vaddr;
            uint64_t size = phdr.mem_sz;

            if (size % PAGE_SIZE_4K != 0)
                size = size - (size % PAGE_SIZE_4K) + PAGE_SIZE_4K;

            space_alloc_region_at(begin, size / PAGE_SIZE_4K, proc->addr_space);

            void* tmp = page_kernel_alloc4k(1);

            for (uint32_t i = 0; i < size; i += PAGE_SIZE_4K)
            {
                void* phys = pmm_request();
                page_kernel_map_memory(tmp, phys, 1);
                memcpy(tmp, data + phdr.offset + i, PAGE_SIZE_4K);
                page_map_memory(phdr.vaddr + i, phys, 1, proc->addr_space);
            }

            page_kernel_free4k(tmp, 1);
        }
    }

    info->entry = hdr.entry;
    return 0;
}