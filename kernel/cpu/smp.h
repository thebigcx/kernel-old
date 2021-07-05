#pragma once

#include <cpu/gdt.h>
#include <cpu/tss.h>

typedef struct cpu
{
    uint32_t lapic_id;
    gdt_entry_t* gdt;
    gdt_ptr_t gdtptr;
    tss_ent_t tss;

} cpu_t;

extern cpu_t cpus[64];

void smp_init();
void smp_initcpu(uint32_t id);