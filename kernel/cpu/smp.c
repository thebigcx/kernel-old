#include <cpu/smp.h>
#include <acpi/acpi.h>
#include <intr/apic.h>
#include <time/time.h>
#include <mem/paging.h>
#include <cpu/gdt.h>

volatile uint64_t* smp_cr3 = 0x1000;
volatile uint64_t* smp_gdt = 0x1008;
volatile uint64_t* smp_ent = 0x1010;

extern void* _ap_bootstrap_start;
extern void* _ap_bootstrap_end;

void smp_entry(uint16_t id)
{
    console_printf("Hello from another core!\n", 255, 255, 255);
}

void smp_init()
{
    *smp_cr3 = page_get_kpml4();
    *smp_gdt = &gdt_ptr;
    *smp_ent = smp_entry;

    memcpy(0x2000, &_ap_bootstrap_start, &_ap_bootstrap_end - &_ap_bootstrap_start);

    uint32_t loc_id = lapic_read(LAPIC_ID) >> 24;

    for (uint32_t i = 0; i < acpi_cpu_cnt; i++)
    {
        uint32_t id = acpi_cpus[i];
        if (id != loc_id)
        {
            console_printf("Initializing core...\n", 255, 255, 255);
            lapic_send_init(id);
        }
    }

    pit_waitms(10);

    for (uint32_t i = 0; i < acpi_cpu_cnt; i++)
    {
        uint32_t id = acpi_cpus[i];
        if (id != loc_id)
        {
            console_printf("Starting up core...\n", 255, 255, 255);
            lapic_send_start(id, 0x8);
        }
    }

    pit_waitms(1);
}