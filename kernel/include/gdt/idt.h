#pragma once

#include "stdint.h"

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

struct interrupt_frame;
__attribute__((interrupt)) void irq14_handler(struct interrupt_frame* frame);
__attribute__((interrupt)) void irq33_handler(struct interrupt_frame* frame);

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

void idt_init();