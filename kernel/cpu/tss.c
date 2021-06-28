#include <cpu/tss.h>
#include <cpu/gdt.h>
#include <util/stdlib.h>

tss_ent_t ktss;

extern void tss_flush();

void tss_init(uint32_t idx, uint64_t krsp)
{
    memset(&ktss, 0, sizeof(tss_ent_t));
    ktss.rsp0l = krsp & 0xffffffff;
    ktss.rsp0h = krsp >> 32;
    ktss.iomap_base = sizeof(tss_ent_t);

    uint64_t base = &ktss;
    uint64_t lim = &ktss + sizeof(tss_ent_t);

    gdt_set_entry(idx, base, lim, GDT_READ, 0, GDT_RING3, 1, 0);
    gdt_ents[idx].access = 1; // Inidicates TSS?
    gdt_ents[idx].mode64bit = 0; // According to manuals
    gdt_ents[idx].gran = 0; // Limit in bytes, not pages

    tss_flush();
}