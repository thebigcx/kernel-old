#pragma once

#include <util/types.h>
#include <mem/paging.h>

// Makes for cleaner code
#define GDT_RING0   0
#define GDT_RING1   1
#define GDT_RING2   2
#define GDT_RING3   3

// Read/write bit
#define GDT_READ    0
#define GDT_WRITE   1

typedef struct gdt_ptr
{
    uint16_t lim;
    uint64_t base;

} __attribute__((packed)) gdt_ptr_t;

typedef union gdt_entry
{
    struct
    {
        uint16_t limlo;
        uint16_t baselo;
        uint8_t  basemid;
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
        uint8_t  basehi;
    } __attribute__((packed));
    struct
    {
        uint32_t low;
        uint32_t high;
    } __attribute__((packed));

} __attribute__((packed)) gdt_entry_t;

#define GDT_NUM_DESCS 7

__attribute__((aligned(PAGE_SIZE_4K))) extern gdt_entry_t bsp_gdtents[GDT_NUM_DESCS];
extern gdt_ptr_t bsp_gdtptr;

// Task State Segment
typedef struct tss_ent
{
    uint32_t res1;
    uint64_t rsp[3];
    uint64_t res2;
    uint64_t ist[7];
    uint64_t res3;
    uint16_t res4;
    uint16_t iomap_base;

} __attribute__((packed)) tss_t;

void tss_init(tss_t* tss, uint32_t select, gdt_entry_t* gdt);

void gdt_init();
void gdt_set_null(uint32_t idx);
void gdt_mkentry(uint32_t idx, uint32_t base, uint32_t lim, uint8_t rw, uint8_t dc, uint8_t dpl, uint8_t code, uint8_t codedata);

void gdt_set_entry(gdt_entry_t* gdt, uint32_t select, gdt_entry_t* entry);
void gdt_set_raw_entry(gdt_entry_t* gdt, uint32_t select, uint32_t low, uint32_t high);
void gdt_setbase(gdt_entry_t* entry, uint32_t base);
void gdt_setlim(gdt_entry_t* entry, uint32_t lim);

// Defined in assembly
extern void gdt_flush(gdt_ptr_t* ptr);
extern void tss_flush();
