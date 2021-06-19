#pragma once

#include <util/types.h>

// Local APIC registers
#define LAPIC_ID             0x020
#define LAPIC_VERS           0x030
#define LAPIC_TPR            0x080
#define LAPIC_APR            0x090
#define LAPIC_PPR            0x0a0
#define LAPIC_EOI            0x0b0
#define LAPIC_RRD            0x0c0
#define LAPIC_LDR            0x0d0
#define LAPIC_DFR            0x0e0
#define LAPIC_SIVR           0x0f0
#define LAPIC_ISR            0x100
#define LAPIC_TMR            0x180
#define LAPIC_IRR            0x200
#define LAPIC_ERR            0x280
#define LAPIC_ICRLO          0x300
#define LAPIC_ICRHI          0x310

#define LAPIC_LVT_TIME       0x320
#define LAPIC_LVT_THERM      0x330
#define LAPIC_LVT_PERF       0x340
#define LAPIC_LVT_LINT0      0x350
#define LAPIC_LVT_LINT1      0x360
#define LAPIC_LVT_ERR        0x370

#define LAPIC_TIME_INIT      0x380
#define LAPIC_TIME_CURR      0x390
#define LAPIC_TIME_DIVCFG    0x3e0

#define LAPIC_BASE           0xffffffffff000

// Delivery mode
#define ICR_FIXED               0x000
#define ICR_LOWEST              0x100
#define ICR_SMI                 0x200
#define ICR_NMI                 0x400
#define ICR_INIT                0x500
#define ICR_STRTUP              0x600

// Delivery status
#define ICR_IDLE                0x0000
#define ICR_SEND_PEND           0x1000

// Level
#define ICR_PHYS                0x0000
#define ICR_LOG                 0x1000

// Trigger mode
#define ICR_DEASSERT            0x0000
#define ICR_ASSERT              0x4000
#define ICR_EDGE                0x0000
#define ICR_LEVEL               0x8000

// Destination shorthand
#define ICR_NO_SHORT            0x00000
#define ICR_SELF                0x40000
#define ICR_ALL_INC_SELF        0x80000
#define ICR_ALL_EX_SELF         0xc0000

// Destination field
#define ICR_DST_SHFT            24

// IO/APIC registers
#define IOAPIC_REGSEL           0x00
#define IOAPIC_WIN              0x10
#define IOAPIC_ID               0x00
#define IOAPIC_VERS             0x01
#define IOAPIC_ARB              0x02

#define IOAPIC_RED_TBL_ENT(x) (0x10 + 2 * x)

void apic_init();

uint32_t ioapic_read32(uint32_t reg);
uint64_t ioapic_read64(uint32_t reg);
void ioapic_write32(uint32_t reg, uint32_t val);
void ioapic_write64(uint32_t reg, uint64_t val);
void ioapic_redirect(uint8_t irq, uint8_t vec, uint32_t delivery);
void ioapic_map_irq(uint8_t irq);
void ioapic_set_base(uint64_t base);

void lapic_write(uint32_t off, uint32_t val);
uint32_t lapic_read(uint32_t off);
void lapic_send_init(uint32_t apicid);
void lapic_send_start(uint32_t apicid, uint32_t vec);

void lapic_eoi();