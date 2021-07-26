#pragma once

#include <util/types.h>
#include <arch/x86_64/system.h>

#define IDT_PRESENT             (1 << 7)
#define IDT_KERNEL              (0)
#define IDT_USER                (3)
#define IDT_STOR                (0)
#define IDT_TYPE_TASK           (0x5)
#define IDT_TYPE_INT            (0xe)
#define IDT_TYPE_TRAP           (0xf)

#define IDT_TA_INT_GATE         0x8e
#define IDT_TA_CALL_GATE        0x8c
#define IDT_TA_TRAP_GATE        0x8f
#define IDT_TA_USER_INT_GATE    0xee

#define IPI_SCHED               0xfd
#define IPI_HALT                0xfe

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

typedef void (*int_fn_t)(reg_ctx_t* r);

void idt_init();
void idt_flush();
void idt_set_int(uint32_t id, int_fn_t fn);
void generic_isr(const char* err, isr_frame_t* frame);
void pagefault_handler(isr_frame_t* frame);
void general_protection_fault_handler(isr_frame_t* frame);
