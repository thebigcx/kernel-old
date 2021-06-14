#pragma once

#include <types.h>
#include <paging/paging.h>

// Access byte
#define GDTA_PRESENT  (1 << 7)
#define GDTA_CODEDATA (1 << 4)
#define GDTA_EXEC     (1 << 3)
#define GDTA_DC       (1 << 2)
#define GDTA_WRITABLE (1 << 1)
#define GDTA_ACCESS   (1 << 0)

// Flags byte
#define GDTF_GRAN     (1 << 7)
#define GDTF_SIZE     (1 << 6)
#define GDTF_LONG     (1 << 5)

typedef struct gdt_desc
{
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) gdt_desc_t;

typedef struct gdt_entry
{
    uint16_t lim0;
    uint16_t base0;
    uint8_t base1;
    uint8_t access;
    uint8_t lim1_flags;
    uint8_t base2;
} __attribute__((packed)) gdt_entry_t;

typedef struct gdt
{
    gdt_entry_t null;
    gdt_entry_t kernel_code;
    gdt_entry_t kernel_data;
    gdt_entry_t user_null;
    gdt_entry_t user_code;
    gdt_entry_t user_data;
} __attribute__((packed)) 
  __attribute__((aligned(PAGE_SIZE))) gdt_t;

extern gdt_t def_gdt;
// Defined in assembly
extern void gdt_load(gdt_desc_t* gdt_desc);