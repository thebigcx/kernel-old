#include <cpu/smp.h>
#include <acpi/acpi.h>
#include <intr/apic.h>
#include <time/time.h>

void smp_init()
{
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