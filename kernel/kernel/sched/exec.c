#include <sched/sched.h>
#include <elf.h>
#include <stddef.h>
#include <stdbool.h>
#include <paging/paging.h>
#include <mem/heap.h>
// TEMP
#include <console.h>

bool check_elf_hdr(elf64_hdr_t* hdr)
{
    if (hdr->ident[0] != ELFMAG0 ||
        hdr->ident[1] != ELFMAG1 ||
        hdr->ident[2] != ELFMAG2 ||
        hdr->ident[3] != ELFMAG3)
    {
        
        return false;
    }

    if (hdr->ident[EI_CLASS] != ELFCLASS64) return false;

    return true;
}

proc_t* mk_elf_proc(uint8_t* elf_dat)
{
    elf64_hdr_t hdr;

    memcpy(&hdr, elf_dat, sizeof(elf64_hdr_t));

    if (!check_elf_hdr(&hdr))
        return NULL;

    uint64_t exec_base = 0;

    for (uint16_t i = 0; i < hdr.ph_num; i++)
    {
        elf64_phdr_t phdr;
        memcpy(&phdr, elf_dat + hdr.ph_off + hdr.ph_ent_size * i, sizeof(elf64_phdr_t));

        if (phdr.type == PT_LOAD)
        {
            exec_base = kmalloc(phdr.mem_sz);

            memcpy((void*)(exec_base + phdr.vaddr), elf_dat + phdr.offset, phdr.file_sz);
        }
    }

    //char buffer[100];
    //console_write(itoa(*(uint8_t*)(exec_base), buffer, 16), 255, 0, 0);
    //while (1);

    return mk_proc(hdr.entry + exec_base);
}