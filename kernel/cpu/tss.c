#include <cpu/tss.h>
#include <cpu/gdt.h>
#include <util/stdlib.h>

extern void tss_flush();
extern void* stack;

void tss_init(tss_t* tss, uint32_t select, gdt_entry_t* gdt)
{
    memset(tss, 0, sizeof(tss_t));

    uint64_t base = tss;
    uint64_t lim = sizeof(tss_t) - 1;

    gdt_entry_t ent;
    memset(&ent, 0, sizeof(gdt_entry_t));

    gdt_setlim(&ent, sizeof(tss_t) - 1);
    gdt_setbase(&ent, ((uint64_t)tss) & 0xffffffff);

    ent.access = 1; // Inidicates TSS?
    //ent.mode64bit = 1; // According to manuals
    ent.present = 1;
    ent.code = 1;
    ent.dpl = 3;
    
    gdt_set_entry(gdt, 5, &ent);

    gdt[6].low = (uint64_t)tss >> 32;
    gdt[6].high = 0;

    tss->rsp[0] = (uint64_t)&stack;
    tss->iomap_base = sizeof(tss_t);

    tss_flush();
}