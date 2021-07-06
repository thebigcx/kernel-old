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
    gdt_setbase(&ent, tss);

    ent.access = 1; // Inidicates TSS?
    ent.mode32bit = 1; // According to manuals
    ent.present = 1;
    ent.code = 1;
    
    gdt_set_entry(gdt, 5, &ent);

    gdt[6].low = (uint64_t)tss >> 32;

    tss->rsp0l = (uint64_t)stack & 0xffffffff;
    tss->rsp0h = (uint64_t)stack >> 32;
    tss->iomap_base = sizeof(tss_t);

    tss_flush();
    serial_writestr("TSS");
}