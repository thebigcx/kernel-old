#include <arch/x86_64/gdt.h>
#include <util/stdlib.h>

__attribute__((aligned(PAGE_SIZE_4K))) gdt_entry_t bsp_gdtents[GDT_NUM_DESCS];
gdt_ptr_t bsp_gdtptr;

void gdt_init()
{
    gdt_set_null(0);

    //gdt_set_raw_entry(bsp_gdtents, 1, 0xffffffff, 0x00af9a00);
    //gdt_set_raw_entry(bsp_gdtents, 2, 0xffffffff, 0x00af9200);
    //gdt_mkentry(2, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING0, 0, 1);

    //gdt_set_raw_entry(bsp_gdtents, 3, 0xffffffff, 0x00affa00);
    //gdt_set_raw_entry(bsp_gdtents, 4, 0xffffffff, 0x00aff200);
    gdt_mkentry(1, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING0, 1, 1);
    gdt_mkentry(2, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING0, 0, 1);

    gdt_mkentry(3, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING3, 1, 1);
    gdt_mkentry(4, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING3, 0, 1);

    bsp_gdtptr.lim = sizeof(bsp_gdtents) - 1;
    bsp_gdtptr.base = (uint64_t)bsp_gdtents;

    gdt_flush(&bsp_gdtptr);
}

void gdt_set_null(uint32_t idx)
{
    memset(&bsp_gdtents[idx], 0, sizeof(gdt_entry_t));
}

void gdt_mkentry(uint32_t idx, uint32_t base, uint32_t lim, uint8_t rw, uint8_t dc, uint8_t dpl, uint8_t code, uint8_t codedata)
{
    gdt_entry_t* ent = &bsp_gdtents[idx];
    ent->limlo = lim & 0xffff;
    ent->limhi = (lim >> 16) & 0x0f;
    ent->baselo = base & 0xffffff;
    ent->basehi = (base >> 24) & 0xff;
    ent->access = 0;
    ent->rw = rw & 0x01;
    ent->dc = dc & 0x01;
    ent->code = code & 0x01;
    ent->codedata = codedata & 0x01;
    ent->dpl = dpl & 0x3;
    ent->present = 1;
    ent->avail = 0;
    ent->mode64bit = 1;
    ent->mode32bit = 0;
    ent->gran = 1;
}

void gdt_set_raw_entry(gdt_entry_t* gdt, uint32_t select, uint32_t low, uint32_t high)
{
    gdt[select].low = low;
    gdt[select].high = high;
}

void gdt_set_entry(gdt_entry_t* gdt, uint32_t select, gdt_entry_t* entry)
{
    gdt_set_raw_entry(gdt, select, entry->low, entry->high);
}

void gdt_setbase(gdt_entry_t* entry, uint32_t base)
{
    entry->baselo = base & 0xffff;
    entry->basemid = (base >> 16) & 0xff;
    entry->basehi = (base >> 24) & 0xff;
}

void gdt_setlim(gdt_entry_t* entry, uint32_t lim)
{
    entry->limlo = lim & 0xffff;
    entry->limhi = (lim >> 16) & 0x0f;
}

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
    ent.present = 1;
    ent.code = 1;
    ent.dpl = 3;
    
    gdt_set_entry(gdt, 5, &ent);

    gdt[6].low = (uint64_t)tss >> 32;
    gdt[6].high = 0;

    tss->rsp[0] = (uint64_t)&stack + 4096; // Stack top
    tss->iomap_base = sizeof(tss_t);

    tss_flush();
}
