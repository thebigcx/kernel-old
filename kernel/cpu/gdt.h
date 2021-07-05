#pragma once

#include <util/types.h>
#include <mem/paging.h>

// Access byte
/*#define GDTA_PRESENT    (1 << 7)
#define GDTA_DATA       (1 << 4)
#define GDTA_CODE       (1 << 3)
#define GDTA_DC         (1 << 2)
#define GDTA_WRITABLE   (1 << 1)
#define GDTA_ACCESS     (1 << 0)

// Flags byte
#define GDTF_GRAN     (1 << 7)
#define GDTF_SIZE     (1 << 6)
#define GDTF_LONG     (1 << 5)*/

// Makes for cleaner code
#define GDT_RING0   0
#define GDT_RING1   1
#define GDT_RING2   2
#define GDT_RING3   3

// Read/write bit
#define GDT_READ    0
#define GDT_WRITE   1

/*typedef struct gdt_desc
{
    uint16_t size;
    uint64_t offset;
} __attribute__((packed)) gdt_desc_t;*/

typedef struct gdt_ptr
{
    uint16_t lim;
    uint64_t base;

} __attribute__((packed)) gdt_ptr_t;

typedef struct gdt_entry
{
    uint32_t limlo      : 16;
    uint32_t baselo     : 24;
    uint32_t access     : 1;
    uint32_t rw         : 1;
    uint32_t dc         : 1;
    uint32_t code       : 1;
    uint32_t codedata   : 1;
    uint32_t dpl        : 2;
    uint32_t present    : 1;
    uint32_t limhi      : 4;
    uint32_t avail      : 1;
    uint32_t mode64bit  : 1;
    uint32_t mode32bit  : 1;
    uint32_t gran       : 1;
    uint32_t basehi     : 8;

    /*uint16_t lim0;
    uint16_t base0;
    uint8_t base1;
    uint8_t access;
    uint8_t lim1_flags;
    uint8_t base2;*/

} __attribute__((packed)) gdt_entry_t;

/*typedef struct gdt
{
    gdt_entry_t null;
    gdt_entry_t kernel_code;
    gdt_entry_t kernel_data;
    gdt_entry_t user_null;
    gdt_entry_t user_code;
    gdt_entry_t user_data;
} __attribute__((packed)) 
  __attribute__((aligned(PAGE_SIZE_4K))) gdt_t;*/

#define GDT_NUM_DESCS 6

__attribute__((aligned(PAGE_SIZE_4K))) extern gdt_entry_t bsp_gdtents[GDT_NUM_DESCS];
extern gdt_ptr_t bsp_gdtptr;

void gdt_init();
void gdt_set_null(uint32_t idx);
void gdt_set_entry(uint32_t idx, uint32_t base, uint32_t lim, uint8_t rw, uint8_t dc, uint8_t dpl, uint8_t code, uint8_t codedata);

//extern gdt_t gdt_def;
//extern gdt_desc_t gdt_desc;
// Defined in assembly
extern void gdt_flush(gdt_ptr_t* ptr);