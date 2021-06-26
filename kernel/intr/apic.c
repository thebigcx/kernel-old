#include <intr/apic.h>
#include <intr/idt.h>
#include <cpu/cpuid.h>
#include <sys/console.h>
#include <mem/paging.h>
#include <acpi/acpi.h>

uint64_t lapic_base;
volatile uint64_t lapic_vbase;

// Spurious interrupt handler
void spur_int()
{
    console_write("Spurious interrupt", 255, 255, 0);
}

uint64_t lapic_read_base()
{
    uint64_t low, high;
    asm ("rdmsr" : "=a"(low), "=d"(high) : "c"(0x1b));

    return ((high << 32) | low);
}

void lapic_set_base(uint64_t val)
{
    uint64_t low = val & 0xffffffff;
    uint64_t high = val >> 32;

    asm ("wrmsr" :: "a"(low), "d"(high), "c"(0x1b));
}

void lapic_init()
{
    lapic_base = lapic_read_base() & LAPIC_BASE;
    lapic_vbase = page_request();

    page_kernel_map_memory((void*)lapic_vbase, (void*)lapic_base);

    idt_set_int(0xff, spur_int);

    lapic_set_base(lapic_read_base() | (1ul << 11));
    lapic_write(LAPIC_SIVR, lapic_read(LAPIC_SIVR) | 0x1ff);
}

void lapic_write(uint32_t off, uint32_t val)
{
    *((volatile uint32_t*)(lapic_vbase + off)) = val;
}

uint32_t lapic_read(uint32_t off)
{
    return *((volatile uint32_t*)(lapic_vbase + off));
}

void lapic_eoi()
{
    lapic_write(LAPIC_EOI, 0);
}

void lapic_send_init(uint32_t apicid)
{
    lapic_write(LAPIC_ICRHI, apicid << ICR_DST_SHFT);
    lapic_write(LAPIC_ICRLO, ICR_INIT | ICR_PHYS | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORT);

    while (lapic_read(LAPIC_ICRLO) & ICR_SEND_PEND);
}

void lapic_send_start(uint32_t apicid, uint32_t vec)
{
    lapic_write(LAPIC_ICRHI, apicid << ICR_DST_SHFT);
    lapic_write(LAPIC_ICRLO, vec | ICR_STRTUP | ICR_PHYS | ICR_ASSERT | ICR_EDGE | ICR_NO_SHORT);

    while (lapic_read(LAPIC_ICRLO) & ICR_SEND_PEND);
}

void lapic_send_ipi(uint8_t apicid, uint32_t dsh, uint32_t type, uint8_t vec)
{
    lapic_write(LAPIC_ICRHI, (uint32_t)apicid << ICR_DST_SHFT);
    lapic_write(LAPIC_ICRLO, dsh | type | vec);

    while (lapic_read(LAPIC_ICRLO) & ICR_SEND_PEND);
}

uint64_t ioapic_base;
uint64_t ioapic_vbase; // Virtual base
volatile uint32_t* regsel;
volatile uint32_t* io_win;

uint32_t apic_ints;
uint32_t apic_id;

void ioapic_init()
{
    ioapic_vbase = page_request();
    page_kernel_map_memory(ioapic_vbase, ioapic_base);

    regsel = (uint32_t*)(ioapic_vbase + IOAPIC_REGSEL);
    io_win = (uint32_t*)(ioapic_vbase + IOAPIC_WIN);

    apic_ints = ioapic_read32(IOAPIC_VERS) >> 16;
    apic_id = ioapic_read32(IOAPIC_ID) >> 24;

    for (uint32_t i = 0; i < acpi_isos.cnt; i++)
    {
        apic_iso_t* iso = acpi_isos.data[i];
        ioapic_redirect(iso->gsi, iso->irq_src + 32, ICR_LOWEST);
    }
}

void ioapic_set_base(uint64_t base)
{
    ioapic_base = base;
}

void ioapic_map_irq(uint8_t irq)
{
    ioapic_redirect(irq, irq + 32, ICR_LOWEST);
}

void ioapic_redirect(uint8_t irq, uint8_t vec, uint32_t delivery)
{
    ioapic_write64(IOAPIC_RED_TBL_ENT(irq), delivery | vec);
}

uint32_t ioapic_read32(uint32_t reg)
{
    *regsel = reg;
    return *io_win;
}

uint64_t ioapic_read64(uint32_t reg)
{

}

void ioapic_write32(uint32_t reg, uint32_t val)
{
    *regsel = reg;
    *io_win = val;
}

void ioapic_write64(uint32_t reg, uint64_t val)
{
    uint32_t low = val & 0xffffffff;
    uint32_t high = val >> 32;

    ioapic_write32(reg, low);
    ioapic_write32(reg + 1, high);
}

void apic_init()
{
    cpuid_inf_t cpu = cpuid();

    if (!(cpu.edx_feats & CPUID_FEAT_EDX_APIC))
    {
        console_write("CPU does not support APIC\n", 255, 0, 0);
        return;
    }

    cli();

    pic_disable();

    lapic_init();
    ioapic_init();

    sti();
}