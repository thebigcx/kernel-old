#pragma once

#include <stdint.h>
#include <stdbool.h>

#define ELF_NIDENT 16

typedef struct elf64_hdr
{
    uint8_t   ident[ELF_NIDENT];
    uint16_t  type;
    uint16_t  machine;
    uint32_t  version;
    uint64_t  entry;
    uint64_t  ph_off;
    uint64_t  sh_off;
    uint32_t  flags;
    uint16_t  eh_size;
    uint16_t  ph_ent_size;
    uint16_t  ph_num;
    uint16_t  sh_ent_size;
    uint16_t  sh_num;
    uint16_t  sh_str_idx;

} elf64_hdr_t;

typedef struct elf64_sect_hdr
{
    uint32_t  name;
    uint32_t  type;
    uint32_t  flags;
    uint64_t  addr;
    uint64_t  offset;
    uint32_t  size;
    uint32_t  link;
    uint32_t  info;
    uint32_t  addr_align;
    uint32_t  ent_size;

} elf64_sect_hdr_t;

typedef struct elf64_sym
{
    uint32_t  name;
    uint64_t  value;
    uint32_t  size;
    uint8_t   info;
    uint8_t   other;
    uint16_t  sh_idx;

} elf64_sym_t;

typedef struct elf64_program_hdr
{
    uint32_t  type;
    uint64_t  offset;
    uint64_t  vaddr;
    uint64_t  paddr;
    uint32_t  file_sz;
    uint32_t  mem_sz;
    uint32_t  flags;
    uint32_t  align;

} elf64_program_hdr_t;

#define SHN_UNDEF (0x00)

// ShT types
#define SHT_NULL        0
#define SHT_PROGBITS    1
#define SHT_SIMTAB      2
#define SHT_STRTAB      3
#define SHT_RELA        4
#define SHT_NOBITS      8
#define SHT_REL         9

// ShT attributes
#define SHF_WRITE 0x01
#define SHF_ALLOC 0x02

// Elf identitifers
#define EI_MAG0         0
#define ELFMAG0         0x7f

#define EI_MAG1         1
#define ELFMAG1         'E'

#define EI_MAG2         2
#define ELFMAG2         'L'

#define EI_MAG3         3
#define ELFMAG3         'F'

#define EI_CLASS        4
#define ELFCLASSNONE    0
#define ELFCLASS32      1
#define ELFCLASS64      2
#define ELFCLASSNUM     3

#define EI_DATA         5
#define ELFDATANONE     0
#define ELFDATA2LSB     1
#define ELFDATA2MSB     2
#define ELFDATANUM      3

#define EI_VERSION      6
#define EV_CURRENT      1

#define EI_OSABI        7
#define EI_ABIVERSION   8
#define EI_PAD          9

#define ELFDATA2LSB     (1)
#define ELFCLASS32      (1)

// Elf types
#define ET_NONE         0
#define ET_REL          1
#define ET_EXEC         2

#define EM_X86_64       62

#define ELF32_ST_BIND(INFO) ((INFO) >> 4)
#define ELF32_ST_TYPE(INFO) ((INFO) & 0x0f)

// StT bindings
#define STB_LOCAL       0
#define STB_GLOBAL      1
#define STB_WEAK        2

// StT types
#define STT_NOTYPE      0
#define STT_OBJECT      1
#define STT_FUNC        2

bool elf_check_file(elf64_hdr_t* hdr);
bool elf_check_supported(elf64_hdr_t* hdr);
void* elf_load_rel(elf64_hdr_t* hdr);
void* elf_load_file(void* file);
elf64_sect_hdr_t* elf_sect_hdr(elf64_hdr_t* hdr);
elf64_sect_hdr_t* elf_section(elf64_hdr_t* hdr, uint32_t idx);
char* elf_str_tbl(elf64_hdr_t* hdr);
char* elf_lookup_str(elf64_hdr_t* hdr, uint32_t offset);
int elf_load_stage1(elf64_hdr_t* hdr);
int elf_load_stage2(elf64_hdr_t* hdr);