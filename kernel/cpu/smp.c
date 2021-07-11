#include <cpu/smp.h>
#include <acpi/acpi.h>
#include <intr/apic.h>
#include <time/time.h>
#include <mem/paging.h>
#include <cpu/gdt.h>
#include <drivers/tty/serial.h>
#include <cpu/tss.h>

#define SMP_TRAMPOLINE_ENTRY 0x8000

volatile uint64_t* smp_cr3   = 0x1000;
volatile uint64_t* smp_gdt   = 0x1008;
volatile uint64_t* smp_ent   = 0x1010;
volatile uint64_t* smp_stack = 0x1018;

volatile int ap_initialized = false;

extern void* _ap_bootstrap_start;
extern void* _ap_bootstrap_end;

cpu_t cpus[64];

void smp_entry(uint16_t id)
{
    ap_initialized = true;
    serial_writestr("Hello from another CPU!\n");

    for (;;);
}

void breakpoint_smp()
{

}

void smp_initcpu(uint32_t id)
{
    asm ("mov %%cr3, %0" : "=r"(*smp_cr3));
    *smp_gdt = &bsp_gdtptr;
    *smp_ent = smp_entry;
    *smp_stack = page_kernel_alloc4k(1);
    page_kernel_map_memory(*smp_stack, pmm_request(), 1);
    *smp_stack += PAGE_SIZE_4K; // Top of the 4K stack

    memcpy(SMP_TRAMPOLINE_ENTRY, &_ap_bootstrap_start, PAGE_SIZE_4K);

    serial_writestr("Sending INIT IPI\n");
    lapic_send_ipi(id, ICR_NO_SHORT, ICR_INIT, 0);

    pit_waitms(10);

    serial_writestr("Sending STARTUP IPI\n");
    lapic_send_ipi(id, ICR_NO_SHORT, ICR_STRTUP | ICR_ASSERT, SMP_TRAMPOLINE_ENTRY >> 12);
    pit_waitms(1);

    if (!ap_initialized)
    {
        serial_writestr("Resending STARTUP IPI\n");
        // Resend with longer timeout
        lapic_send_ipi(id, ICR_NO_SHORT, ICR_STRTUP | ICR_ASSERT, SMP_TRAMPOLINE_ENTRY >> 12);
        pit_waitms(1000);

        if (!ap_initialized)
        {
            serial_writestr(ANSI_RED "CPU failed to startup\n" ANSI_WHITE);
        }
    }
}

void smp_init()
{
    uint32_t locid = lapic_read(LAPIC_ID) >> 24;

    cpus[0].gdt = bsp_gdtents;
    cpus[0].gdtptr = bsp_gdtptr;
    cpus[0].lapic_id = locid;

    for (uint32_t i = 0; i < acpi_cpu_cnt; i++)
    {
        uint32_t id = acpi_cpus[i];
        if (id != locid)
        {
            smp_initcpu(id);
        }
    }

    tss_init(&cpus[0].tss, 5, cpus[0].gdt);
}