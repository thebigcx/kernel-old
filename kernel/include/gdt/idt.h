#pragma once

#include <stdint.h>

#define IDT_TA_INTERRUPT_GATE 0x8e
#define IDT_TA_CALL_GATE      0x8c
#define IDT_TA_TRAP_GATE      0x8f

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

/*struct interrupt_frame;
__attribute__((interrupt)) void isr0(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr1(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr2(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr3(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr4(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr5(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr6(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr7(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr8(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr9(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr10(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr11(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr12(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr13(struct interrupt_frame* frame);
__attribute__((interrupt)) void isr14(struct interrupt_frame* frame);

__attribute__((interrupt)) void irq0(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq1(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq2(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq3(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq4(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq5(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq6(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq7(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq8(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq9(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq10(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq11(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq12(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq13(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq14(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq15(struct interrupt_frame* frame);*/

typedef struct idt_entry
{
    uint16_t off_lowbits;
    uint16_t select;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t off_midbits;
    uint32_t off_highbits;
    uint32_t ignore;
} idt_entry_t;

typedef struct idt_record
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_record_t;

// Interrupt callbacks
extern void(*isrs[16])();
extern void(*irqs[16])();

void idt_load();
void idt_set_irq(uint32_t id, void(*handler)());
void idt_set_isr(uint32_t id, void(*handler)());