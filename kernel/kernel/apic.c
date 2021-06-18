#include <apic.h>
#include <gdt/idt.h>
#include <cpuid.h>
#include <console.h>
#include <paging/paging.h>
#include <acpi.h>

uint64_t apicloc_base;
volatile uint64_t apicloc_vbase;

// Spurious interrupt handler
void spur_int()
{
    console_write("Spurious interrupt", 255, 255, 0);
}

uint64_t apicloc_read_base()
{
    uint64_t low, high;
    asm ("rdmsr" : "=a"(low), "=d"(high) : "c"(0x1b));

    return ((high << 32) | low);
}

void apicloc_set_base(uint64_t val)
{
    uint64_t low = val & 0xffffffff;
    uint64_t high = val >> 32;

    asm ("wrmsr" :: "a"(low), "d"(high), "c"(0x1b));
}

void apicloc_init()
{
    apicloc_base = apicloc_read_base() & APIC_LOC_BASE;
    apicloc_vbase = page_request();

    page_kernel_map_memory((void*)apicloc_vbase, (void*)apicloc_base);

    idt_set_int(0xff, spur_int);

    apicloc_set_base(apicloc_read_base() | (1ul << 11));
    apicloc_write(APIC_LOC_SIVR, apicloc_read(APIC_LOC_SIVR) | 0x1ff);
}

void apicloc_write(uint32_t off, uint32_t val)
{
    *((volatile uint32_t*)(apicloc_vbase + off)) = val;
}

uint32_t apicloc_read(uint32_t off)
{
    return *((volatile uint32_t*)(apicloc_vbase + off));
}

void apicloc_eoi()
{
    apicloc_write(APIC_LOC_EOI, 0);
}

uint64_t apicio_base;
uint64_t apicio_vbase; // Virtual base
volatile uint32_t* regsel;
volatile uint32_t* io_win;

uint32_t apic_ints;
uint32_t apic_id;

void apicio_init()
{
    apicio_vbase = page_request();
    page_kernel_map_memory(apicio_vbase, apicio_base);

    regsel = (uint32_t*)(apicio_vbase + APICIO_REGSEL);
    io_win = (uint32_t*)(apicio_vbase + APICIO_WIN);

    apic_ints = apicio_read32(APICIO_VERS) >> 16;
    apic_id = apicio_read32(APICIO_ID) >> 24;

    for (uint32_t i = 0; i < acpi_isos.cnt; i++)
    {
        apic_iso_t* iso = acpi_isos.data[i];
        apicio_redirect(iso->gsi, iso->irq_src + 32, ICR_MSG_TYPE_LOW_PRIORITY);
    }
}

void apicio_set_base(uint64_t base)
{
    apicio_base = base;
}

void apicio_map_irq(uint8_t irq)
{
    apicio_redirect(irq, irq + 32, ICR_MSG_TYPE_LOW_PRIORITY);
}

void apicio_redirect(uint8_t irq, uint8_t vec, uint32_t delivery)
{
    apicio_write64(APICIO_RED_TBL_ENT(irq), delivery | vec);
}

uint32_t apicio_read32(uint32_t reg)
{
    *regsel = reg;
    return *io_win;
}

uint64_t apicio_read64(uint32_t reg)
{

}

void apicio_write32(uint32_t reg, uint32_t val)
{
    *regsel = reg;
    *io_win = val;
}

void apicio_write64(uint32_t reg, uint64_t val)
{
    uint32_t low = val & 0xffffffff;
    uint32_t high = val >> 32;

    apicio_write32(reg, low);
    apicio_write32(reg + 1, high);
}

void apic_init()
{
    cpuid_inf_t cpu = cpuid();

    if (!(cpu.edx_feats & CPUID_FEAT_EDX_APIC))
    {
        console_write("CPU does not support APIC\n", 255, 0, 0);
        return;
    }

    //cli();

    idt_disable_pic();

    apicloc_init();
    apicio_init();

    //sti();
}