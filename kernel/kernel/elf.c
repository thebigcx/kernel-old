#include <elf.h>
#include <stdio.h>

/*bool elf_check_file(elf64_hdr_t* hdr)
{
    if (!hdr) return false;

    if (hdr->ident[EI_MAG0] != ELFMAG0)
    {
        puts("ELF Header EI_MAG0 incorrect.\n");
        return false;
    }
    if (hdr->ident[EI_MAG1] != ELFMAG1)
    {
        puts("ELF Header EI_MAG1 incorrect.\n");
        return false;
    }
    if (hdr->ident[EI_MAG2] != ELFMAG2)
    {
        puts("ELF Header EI_MAG2 incorrect.\n");
        return false;
    }
    if (hdr->ident[EI_MAG3] != ELFMAG3)
    {
        puts("ELF Header EI_MAG3 incorrect.\n");
        return false;
    }

    return true;
}

bool elf_check_supported(elf64_hdr_t* hdr)
{
    if (!elf_check_file(hdr))
    {
        puts("Invalid ELF file.\n");
        return false;
    }
    if (hdr->ident[EI_CLASS] != ELFCLASS64)
    {
        puts("Unsupported ELF File Class.\n");
        return false;
    }
    if (hdr->ident[EI_DATA] != ELFDATA2LSB)
    {
        puts("Unsupported ELF File byte order.\n");
        return false;
    }
    if (hdr->machine != EM_X86_64)
    {
        puts("Unsupported ELF File target.\n");
        return false;
    }
    if (hdr->ident[EI_VERSION] != EV_CURRENT)
    {
        puts("Unsupported ELF File version.\n");
        return false;
    }
    if (hdr->type != ET_REL && hdr->type != ET_EXEC)
    {
        puts("Unsupported ELF File type.\n");
        return false;
    }

    return true;
}

void* elf_load_rel(elf64_hdr_t* hdr)
{
    int result = elf_load_stage1(hdr);

    if (result == ELF_RELOC_ERR)
    {
        puts("Unable to load ELF file.\n");
        return NULL;
    }

    result = elf_load_stage2(hdr);

    if (result == ELF_RELOC_ERR)
    {
        puts("Unable to load elf file.\n");
        return NULL;
    }

    return (void*)hdr->entry;
}

void* elf_load_file(void* file)
{
    elf64_hdr_t* hdr = (elf64_hdr_t*)file;

    if (!elf_check_supported(hdr))
    {
        puts("ELF file cannot be loaded.\n");
        return NULL;
    }

    switch (hdr->type)
    {
        // Not loading ET_EXEC just yet, won't be used.
        case ET_REL:
            return elf_load_rel(hdr);
    }

    return NULL;
}

elf64_sect_hdr_t* elf_sect_hdr(elf64_hdr_t* hdr)
{
    return (elf64_sect_hdr_t*)((uint64_t)hdr + hdr->sh_off);
}

elf64_sect_hdr_t* elf_section(elf64_hdr_t* hdr, uint32_t idx)
{
    return &elf_sect_hdr(hdr)[idx];
}

char* elf_str_tbl(elf64_hdr_t* hdr)
{
    if (hdr->sh_str_idx == SHN_UNDEF) return NULL;

    return (char*)hdr + elf_section(hdr, hdr->sh_str_idx)->offset;
}

char* elf_lookup_str(elf64_hdr_t* hdr, uint32_t offset)
{
    char* str_tbl = elf_str_tbl(hdr);
    if (str_tbl == NULL) return NULL;

    return str_tbl + offset;
}

int elf_load_stage1(elf64_hdr_t* hdr)
{

}

int elf_load_stage2(elf64_hdr_t* hdr)
{

}*/