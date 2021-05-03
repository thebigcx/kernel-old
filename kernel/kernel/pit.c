#include <pit.h>
#include <gdt/idt.h>
#include <stdio.h>
#include <io.h>

static uint32_t curr_count;

void pit_int_handler()
{
    puts("PIT tick\n");
}

void pit_init()
{
    idt_set_irq(0, pit_int_handler);
}

void pit_set_count(uint32_t cnt)
{
    curr_count = cnt;

    outb(PIT_COMMAND, 0x36);

    outb(PIT_CHANNEL0, cnt & 0xff);
    iowait();
    outb(PIT_CHANNEL0, cnt >> 8);
}