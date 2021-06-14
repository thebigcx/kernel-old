#include <time/time.h>
#include <gdt/idt.h>

#include <io.h>
#include <sched/sched.h>
#include <apic.h>

uint64_t ticks;
uint64_t freq;

void pit_int_handler(reg_ctx_t* r)
{
    ticks++;
    
    sched_tick(r);
}

void pit_init(uint64_t frequency)
{
    idt_set_int(32, pit_int_handler);

    freq = frequency;
    uint32_t div = PIT_FREQ / freq;

    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, div & 0xff);
    outb(PIT_CHANNEL0, div >> 8);
}

time_t pit_boot_time()
{
    return ticks * 1000000 / freq;
}