#include <gdt/idt.h>
#include <io.h>
#include <string.h>
#include <paging/paging.h>

static void set_handler(idt_entry_t* entry, void* fn)
{
    entry->off_lowbits  = (uint16_t)(((uint64_t)fn & 0x000000000000ffff));
    entry->off_midbits  = (uint16_t)(((uint64_t)fn & 0x00000000ffff0000) >> 16);
    entry->off_highbits = (uint32_t)(((uint64_t)fn & 0xffffffff00000000) >> 32);
}

void idt_init()
{
    idt_entry_t* idt = page_request();
    memset(idt, 0, sizeof(idt_entry_t) * 256);

    set_handler(&idt[14], irq14_handler);
    idt[14].select = 0x08;
    idt[14].type_attr = IDT_TA_INTERRUPT_GATE;

    set_handler(&idt[33], irq33_handler);
    idt[33].select = 0x08;
    idt[33].type_attr = IDT_TA_INTERRUPT_GATE;

    idt_record_t idtr;
    idtr.limit = 256 * sizeof(idt_entry_t) - 1;
    idtr.base = (uint64_t)idt;

    asm ("lidt %0" :: "m"(idtr));

    uint8_t a1, a2;
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    // Remap PIC
    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);
    outb(PIC1_DATA, 0x20);
    outb(PIC2_DATA, 0x28);
    outb(PIC1_DATA, 0x04);
    outb(PIC2_DATA, 0x02);
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC2_DATA, ICW4_8086);
    outb(PIC1_DATA, a1);
    outb(PIC2_DATA, a2);

    outb(PIC1_DATA, 0xfd);
    outb(PIC2_DATA, 0xff);

    asm ("sti");
}