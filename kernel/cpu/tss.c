#include <cpu/tss.h>
#include <cpu/gdt.h>
#include <util/stdlib.h>

extern void tss_flush();

void tss_init(tss_ent_t* tss, uint32_t select, gdt_entry_t* gdt)
{
    //memset(&ktss, 0, sizeof(tss_ent_t));
    //ktss.rsp0l = krsp & 0xffffffff;
    //ktss.rsp0h = krsp >> 32;
    //ktss.iomap_base = sizeof(tss_ent_t);

    uint64_t base = tss;
    uint64_t lim = tss + sizeof(tss_ent_t);

    gdt_set_entry(select, base, lim, GDT_READ, 0, GDT_RING3, 1, 0);
    gdt[select].access = 1; // Inidicates TSS?
    gdt[select].mode64bit = 0; // According to manuals
    gdt[select].gran = 0; // Limit in bytes, not pages

    tss_flush();
}