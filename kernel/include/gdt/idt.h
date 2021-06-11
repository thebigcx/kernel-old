#pragma once

#include <stdint.h>

#define IDT_PRESENT     (1 << 7)
#define IDT_KERNEL      (0)
#define IDT_USER        (3)
#define IDT_STOR        (0)
#define IDT_TYPE_TASK   (0x5)
#define IDT_TYPE_INT    (0xe)
#define IDT_TYPE_TRAP   (0xf)

#define IDT_TA_INT_GATE         0x8e
#define IDT_TA_CALL_GATE        0x8c
#define IDT_TA_TRAP_GATE        0x8f
#define IDT_TA_USER_INT_GATE    0xee

#define PIC1_COMMAND 0x20
#define PIC1_DATA    0x21
#define PIC2_COMMAND 0xA0
#define PIC2_DATA    0xA1
#define PIC_EOI      0x20

#define ICW1_INIT    0x10
#define ICW1_ICW4    0x01
#define ICW4_8086    0x01

typedef struct idt_entry
{
    uint16_t off_lowbits;
    uint16_t select;
    uint8_t ist;

    uint8_t type    : 4;
    uint8_t stor    : 1;
    uint8_t dpl     : 2;
    uint8_t present : 1;

    uint16_t off_midbits;
    uint32_t off_highbits;
    uint32_t ignore;

} __attribute__((packed)) idt_entry_t;

typedef struct idt_record
{
    uint16_t limit;
    uint64_t base;
} __attribute__((packed)) idt_record_t;

// Interrupt callbacks
extern void(*isrs[16])();
extern void(*irqs[16])();

void idt_load();
void idt_set_int(uint32_t id, void(*handler)());
void idt_disable_pic();