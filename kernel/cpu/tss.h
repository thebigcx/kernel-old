#pragma once

#include <util/types.h>
#include <cpu/gdt.h>

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