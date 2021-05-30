#include <gdt/idt.h>
#include <io.h>
#include <string.h>
#include <paging/paging.h>
#include <system.h>

extern void isr0();
extern void isr1();
extern void isr2();
extern void isr3();
extern void isr4();
extern void isr5();
extern void isr6();
extern void isr7();
extern void isr8();
extern void isr9();
extern void isr10();
extern void isr11();
extern void isr12();
extern void isr13();
extern void isr14();
extern void isr15();
extern void isr16();
extern void isr17();
extern void isr18();
extern void isr19();
extern void isr20();
extern void isr21();
extern void isr22();
extern void isr23();
extern void isr24();
extern void isr25();
extern void isr26();
extern void isr27();
extern void isr28();
extern void isr29();
extern void isr30();
extern void isr31();

extern void irq0();
extern void irq1();
extern void irq2();
extern void irq3();
extern void irq4();
extern void irq5();
extern void irq6();
extern void irq7();
extern void irq8();
extern void irq9();
extern void irq10();
extern void irq11();
extern void irq12();
extern void irq13();
extern void irq14();
extern void irq15();

void(*int_handlers[256])(reg_ctx_t* r);

static void set_handler(idt_entry_t* entry, void* fn)
{
    entry->off_lowbits  = (uint16_t)(((uint64_t)fn & 0x000000000000ffff));
    entry->off_midbits  = (uint16_t)(((uint64_t)fn & 0x00000000ffff0000) >> 16);
    entry->off_highbits = (uint32_t)(((uint64_t)fn & 0xffffffff00000000) >> 32);
    entry->select = 0x08;
    entry->type_attr = IDT_TA_INTERRUPT_GATE;
}

idt_entry_t* idt;

void idt_load()
{
    idt = kmalloc(sizeof(idt_entry_t) * 256);
    memset(idt, 0, sizeof(idt_entry_t) * 256);

    set_handler(&idt[0x0], isr0);
    set_handler(&idt[0x1], isr1);
    set_handler(&idt[0x2], isr2);
    set_handler(&idt[0x3], isr3);
    set_handler(&idt[0x4], isr4);
    set_handler(&idt[0x5], isr5);
    set_handler(&idt[0x6], isr6);
    set_handler(&idt[0x7], isr7);
    set_handler(&idt[0x8], isr8);
    set_handler(&idt[0x9], isr9);
    set_handler(&idt[0xa], isr10);
    set_handler(&idt[0xb], isr11);
    set_handler(&idt[0xc], isr12);
    set_handler(&idt[0xd], isr13);
    set_handler(&idt[0xe], isr14);
    set_handler(&idt[0xf], isr15);
    set_handler(&idt[0x10], isr16);
    set_handler(&idt[0x11], isr17);
    set_handler(&idt[0x12], isr18);
    set_handler(&idt[0x13], isr19);
    set_handler(&idt[0x14], isr20);
    set_handler(&idt[0x15], isr21);
    set_handler(&idt[0x16], isr22);
    set_handler(&idt[0x17], isr23);
    set_handler(&idt[0x18], isr24);
    set_handler(&idt[0x19], isr25);
    set_handler(&idt[0x1a], isr26);
    set_handler(&idt[0x1b], isr27);
    set_handler(&idt[0x1c], isr28);
    set_handler(&idt[0x1d], isr29);
    set_handler(&idt[0x1e], isr30);
    set_handler(&idt[0x1f], isr31);

    set_handler(&idt[0x20], irq0);
    set_handler(&idt[0x21], irq1);
    set_handler(&idt[0x22], irq2);
    set_handler(&idt[0x23], irq3);
    set_handler(&idt[0x24], irq4);
    set_handler(&idt[0x25], irq5);
    set_handler(&idt[0x26], irq6);
    set_handler(&idt[0x27], irq7);
    set_handler(&idt[0x28], irq8);
    set_handler(&idt[0x29], irq9);
    set_handler(&idt[0x2a], irq10);
    set_handler(&idt[0x2b], irq11);
    set_handler(&idt[0x2c], irq12);
    set_handler(&idt[0x2d], irq13);
    set_handler(&idt[0x2e], irq14);
    set_handler(&idt[0x2f], irq15);

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
}

void idt_set_irq(uint32_t id, void(*handler)(reg_ctx_t* r))
{
    int_handlers[id + 32] = handler;
}

void idt_set_isr(uint32_t id, void(*handler)(reg_ctx_t* r))
{
    int_handlers[id] = handler;
}

void irq_handler(uint64_t num, reg_ctx_t* r)
{
    if (int_handlers[num])
        int_handlers[num](r);
}

void isr_handler(uint64_t num, reg_ctx_t* r)
{
    if (int_handlers[num])
        int_handlers[num](r);
}