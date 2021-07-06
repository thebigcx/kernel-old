#pragma once

#include <cpu/gdt.h>
#include <cpu/tss.h>

typedef struct cpu
{
    uint32_t lapic_id;
    gdt_entry_t* gdt;
    gdt_ptr_t gdtptr;
    tss_t tss;

} cpu_t;

#define RFLAG_CARRY     0x0001
#define RFLAG_PARITY    0x0004
#define RFLAG_ADJUST    0x0010
#define RFLAG_ZERO      0x0040
#define RFLAG_SIGN      0x0080
#define RFLAG_TRAP      0x0100
#define RFLAG_INTR      0x0200
#define RFLAG_DIR       0x0400
#define RFLAG_OVERFLOW  0x0800