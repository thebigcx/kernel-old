#include <cpu/smp.h>
#include <acpi/acpi.h>
#include <intr/apic.h>
#include <time/time.h>
#include <mem/paging.h>
#include <cpu/gdt.h>
#include <drivers/tty/serial.h>
#include <cpu/tss.h>
#include <intr/idt.h>

#define SMP_TRAMPOLINE_ENTRY 0x8000

volatile uint64_t* smp_cr3   = 0x1000;
volatile uint64_t* smp_gdt   = 0x1008;
volatile uint64_t* smp_ent   = 0x1010;
volatile uint64_t* smp_stack = 0x1018;

volatile int ap_initialized = false;

extern void* _ap_bootstrap_start;
extern void* _ap_bootstrap_end;

cpu_t cpus[64];
uint32_t cpu_count = 0;

void smp_entry(uint16_t id)
{
    cpus[id].lapic_id = id;
    cpus[id].gdt = kmalloc(sizeof(bsp_gdtents)); // lim is subtracted by 1
    memcpy(cpus[id].gdt, bsp_gdtents, sizeof(bsp_gdtents));
    cpus[id].gdtptr.base = cpus[id].gdt;
    cpus[id].gdtptr.lim = sizeof(bsp_gdtents) - 1;

    asm volatile ("lgdt (%%rax)" :: "a"(&cpus[id].gdtptr));
    
    idt_flush();
    
    tss_init(&cpus[id].tss, 5, cpus[id].gdt);

    lapic_enable();

    cpus[id].threads = list_create();
    cpus[id].run_queue = list_create();
    cpus[id].currthread = NULL;
    cpus[id].lock = 0;

    cpu_count++;
    serial_printf("CPU #%d intialized.\n", id);
    ap_initialized = true;

    sti();
    for (;;);
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

    serial_printf("Sending INIT IPI to CPU #%d\n", id);
    lapic_send_ipi(id, ICR_NO_SHORT, ICR_INIT, 0);

    pit_wait(10000000);
    //timer_waitms(10);

    serial_printf("Sending STARTUP IPI to CPU #%d\n", id);
    lapic_send_ipi(id, ICR_NO_SHORT, ICR_STRTUP | ICR_ASSERT, SMP_TRAMPOLINE_ENTRY >> 12);
    pit_wait(1000000);
    //timer_waitus(200);

    if (!ap_initialized)
    {
        serial_printf("Resending STARTUP IPI to CPU #%d\n", id);
        // Resend with longer timeout
        lapic_send_ipi(id, ICR_NO_SHORT, ICR_STRTUP | ICR_ASSERT, SMP_TRAMPOLINE_ENTRY >> 12);
        pit_wait(1000000000);
        //timer_waits(1);

        if (!ap_initialized)
        {
            serial_printf(ANSI_RED "CPU #%d failed to startup\n" ANSI_WHITE, id);
        }
    }
}

void smp_init()
{
    uint32_t locid = lapic_read(LAPIC_ID) >> 24;

    cpus[0].gdt = bsp_gdtents;
    cpus[0].gdtptr = bsp_gdtptr;
    cpus[0].lapic_id = locid;
    cpus[0].run_queue = list_create();
    cpus[0].threads = list_create();
    cpus[0].currthread = NULL;
    cpus[0].lock = 0;
    cpu_count = 1;

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