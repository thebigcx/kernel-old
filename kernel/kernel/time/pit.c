#include <time/time.h>
#include <gdt/idt.h>
#include <stdio.h>
#include <io.h>

uint64_t ticks;
uint64_t freq;

void pit_int_handler()
{
    ticks++;
}

void pit_init(uint64_t frequency)
{
    freq = frequency;
    uint32_t div = PIT_FREQ / freq;

    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, div & 0xff);
    outb(PIT_CHANNEL0, div >> 8);

    idt_set_irq(0, pit_int_handler);
}

time_t pit_boot_time()
{
    return ticks * 1000000 / freq;
}