#include <cpu/gdt.h>
#include <util/stdlib.h>

/*#define GRANLONG GDTF_GRAN | GDTF_LONG

gdt_desc_t gdt_desc;

__attribute__((aligned(PAGE_SIZE_4K)))
gdt_t gdt_def = 
{
    { 0, 0, 0, 0x0,                                                      0x0,      0 }, // Null
    { 0, 0, 0, GDTA_PRESENT | GDTA_CODEDATA | GDTA_EXEC | GDTA_WRITABLE, GRANLONG, 0 }, // Kernel code seg
    { 0, 0, 0, GDTA_PRESENT | GDTA_CODEDATA | GDTA_WRITABLE,             GRANLONG, 0 }, // Kernel data seg
    { 0, 0, 0, 0x0,                                                      0x0,      0 }, // User Null
    { 0, 0, 0, GDTA_PRESENT | GDTA_CODEDATA | GDTA_EXEC | GDTA_WRITABLE, GRANLONG, 0 }, // User code seg
    { 0, 0, 0, GDTA_PRESENT | GDTA_CODEDATA | GDTA_WRITABLE,             GRANLONG, 0 }  // User data seg
};*/
//gdt_t gdt_def;
__attribute__((aligned(PAGE_SIZE_4K))) gdt_entry_t bsp_gdtents[GDT_NUM_DESCS];
gdt_ptr_t bsp_gdtptr;

void gdt_init()
{
    gdt_set_null(0);

    gdt_set_entry(1, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING0, 1, 1);
    gdt_set_entry(2, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING0, 0, 1);

    gdt_set_entry(3, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING3, 1, 1);
    gdt_set_entry(3, 0, 0xffffffff, GDT_WRITE, 0, GDT_RING3, 0, 1);

    bsp_gdtptr.lim = sizeof(bsp_gdtents) - 1;
    bsp_gdtptr.base = (uint64_t)bsp_gdtents;

    gdt_flush(&bsp_gdtptr);
}

void gdt_set_null(uint32_t idx)
{
    memset(&bsp_gdtents[idx], 0, sizeof(gdt_entry_t));
}

void gdt_set_entry(uint32_t idx, uint32_t base, uint32_t lim, uint8_t rw, uint8_t dc, uint8_t dpl, uint8_t code, uint8_t codedata)
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