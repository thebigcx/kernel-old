#include "idt.h"
#include "../io.h"
#include "stdio.h"

__attribute__((interrupt)) void irq14_handler(struct interrupt_frame* frame)
{
    puts("Page fault\n");
    while (1);
}

__attribute__((interrupt)) void irq33_handler(struct interrupt_frame* frame)
{
    puts("Keyboard\n");
    
    outb(PIC1_COMMAND, PIC_EOI);
}