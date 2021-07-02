#include <intr/idt.h>
#include <sys/io.h>
#include <util/stdlib.h>
#include <mem/paging.h>
#include <sys/system.h>
#include <intr/apic.h>
#include <mem/heap.h>

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
extern void isr0x80();

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

extern void ipi0xfd();

int_fn_t int_handlers[256];

static void set_handler(idt_entry_t* entry, void* fn, uint16_t select, uint8_t type, uint8_t dpl)
{
    entry->off_lowbits  = (uint16_t)(((uint64_t)fn & 0x000000000000ffff));
    entry->off_midbits  = (uint16_t)(((uint64_t)fn & 0x00000000ffff0000) >> 16);
    entry->off_highbits = (uint32_t)(((uint64_t)fn & 0xffffffff00000000) >> 32);
    entry->select = select;
    entry->present = true;
    entry->stor = IDT_STOR;
    entry->dpl = dpl;
    entry->type = type;
}

void ipi_halt(reg_ctx_t* regs)
{
    cli();
    asm ("hlt");
}

idt_entry_t idt[256];

void idt_init()
{
    memset(idt, 0, sizeof(idt_entry_t) * 256);

    set_handler(&idt[0], isr0, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[1], isr1, 0x08, IDT_TYPE_TRAP, IDT_KERNEL);
    set_handler(&idt[2], isr2, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[3], isr3, 0x08, IDT_TYPE_TRAP, IDT_KERNEL);
    set_handler(&idt[4], isr4, 0x08, IDT_TYPE_TRAP, IDT_KERNEL);
    set_handler(&idt[5], isr5, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[6], isr6, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[7], isr7, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[8], isr8, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[9], isr9, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[10], isr10, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[11], isr11, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[12], isr12, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[13], isr13, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[14], isr14, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[15], isr15, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[16], isr16, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[17], isr17, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[18], isr18, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[19], isr19, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[20], isr20, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[21], isr21, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[22], isr22, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[23], isr23, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[24], isr24, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[25], isr25, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[26], isr26, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[27], isr27, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[28], isr28, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[29], isr29, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[30], isr30, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[31], isr31, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x80], isr0x80, 0x08, IDT_TYPE_INT, IDT_USER); // syscall

    set_handler(&idt[0x20], irq0, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x21], irq1, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x22], irq2, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x23], irq3, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x24], irq4, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x25], irq5, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x26], irq6, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x27], irq7, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x28], irq8, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x29], irq9, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x2a], irq10, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x2b], irq11, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x2c], irq12, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x2d], irq13, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x2e], irq14, 0x08, IDT_TYPE_INT, IDT_KERNEL);
    set_handler(&idt[0x2f], irq15, 0x08, IDT_TYPE_INT, IDT_KERNEL);

    set_handler(&idt[0xfd], ipi0xfd, 0x08, IDT_TYPE_INT, IDT_KERNEL);

    idt_record_t idtr;
    idtr.limit = 256 * sizeof(idt_entry_t) - 1;
    idtr.base = (uint64_t)idt;

    asm ("lidt %0" :: "m"(idtr));

    idt_set_int(IPI_HALT, ipi_halt);
}

void idt_set_int(uint32_t id, int_fn_t fn)
{
    int_handlers[id] = fn;
}

void irq_handler(uint64_t num, reg_ctx_t* r)
{
    lapic_eoi();

    if (int_handlers[num])
        int_handlers[num](r);
}

void isr_handler(uint64_t num, isr_frame_t* r)
{
    if (num == 0) panic("Divide by zero error", num, r);
    else if (num == 1) console_printf("Debug\n", 255, 255, 255);
    else if (num == 2) panic("Non-maskable interrupt", num, r);
    else if (num == 3) console_printf("Breakpoint\n", 255, 255, 255);
    else if (num == 4) panic("Overflow error", num, r);
    else if (num == 5) panic("Bound range exceeded", num, r);
    else if (num == 6) panic("Invalid opcode", num, r);
    else if (num == 7) panic("Device not available", num, r);
    else if (num == 8) panic("Double fault", num, r);
    else if (num == 9) panic("Coprocessor segment overrun", num, r);
    else if (num == 10) panic("Invalid TSS", num, r);
    else if (num == 11) panic("Segment not present", num, r);
    else if (num == 12) panic("Stack-segment fault", num, r);
    else if (num == 13) panic("General protection fault", num, r);
    else if (num == 14) panic("Page fault", num, r);
    else if (num == 16) panic("x87 Floating-point exception", num, r);
    else if (num == 17) panic("Alignment check", num, r);
    else if (num == 18) panic("Machine check", num, r);
    else if (num == 19) panic("SIMD Floating-point exception", num, r);
    else if (num == 20) panic("Virtualization exception", num, r);
    else if (num == 30) panic("Security exception", num, r);
    else panic("An error with unknown error code has occurred.", num, r);
}

void ipi_handler(uint64_t num, reg_ctx_t* r)
{
    lapic_eoi();

    if (int_handlers[num])
        int_handlers[num](r);
}