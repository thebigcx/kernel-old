#pragma once

#include <types.h>

#define APIC_LOC_ID             0x20
#define APIC_LOC_VERS           0x30
#define APIC_LOC_TPR            0x80
#define APIC_LOC_APR            0x90
#define APIC_LOC_PPR            0xa0
#define APIC_LOC_EOI            0xb0
#define APIC_LOC_RRD            0xc0
#define APIC_LOC_LDR            0xd0
#define APIC_LOC_DFR            0xe0
#define APIC_LOC_SIVR           0xf0
#define APIC_LOC_ISR            0x100
#define APIC_LOC_TMR            0x180
#define APIC_LOC_IRR            0x200
#define APIC_LOC_ERR            0x280
#define APIC_LOC_ICR            0x300

#define APIC_LOC_LVT_TIME       0x320
#define APIC_LOC_LVT_THERM      0x330
#define APIC_LOC_LVT_PERF       0x340
#define APIC_LOC_LVT_LINT0      0x350
#define APIC_LOC_LVT_LINT1      0x360
#define APIC_LOC_LVT_ERR        0x370

#define APIC_LOC_TIME_INIT      0x380
#define APIC_LOC_TIME_CURR      0x390
#define APIC_LOC_TIME_DIVCFG    0x3e0

#define APIC_LOC_BASE           0xffffffffff000

#define ICR_MSG_TYPE_LOW_PRIORITY (1 << 8)

#define APICIO_REGSEL   0x0
#define APICIO_WIN      0x10
#define APICIO_ID       0x0
#define APICIO_VERS     0x1
#define APICIO_ARB      0x2

#define APICIO_RED_TBL_ENT(x) (0x10 + 2 * x)

void apic_init();

uint32_t apicio_read32(uint32_t reg);
uint64_t apicio_read64(uint32_t reg);
void apicio_write32(uint32_t reg, uint32_t val);
void apicio_write64(uint32_t reg, uint64_t val);
void apicio_redirect(uint8_t irq, uint8_t vec, uint32_t delivery);
void apicio_map_irq(uint8_t irq);
void apicio_set_base(uint64_t base);

void apicloc_write(uint32_t off, uint32_t val);
uint32_t apicloc_read(uint32_t off);

void apicloc_eoi();