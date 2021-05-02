#include <gdt/idt.h>
#include <io.h>
#include <stdio.h>
#include <system.h>

__attribute__((interrupt)) void irq14_handler(struct interrupt_frame* frame)
{
    panic("Page fault\n");
    while (1);
}

__attribute__((interrupt)) void irq33_handler(struct interrupt_frame* frame)
{
    uint8_t scan_code = inb(0x60);
    puts("Keyboard\n");
    
    outb(PIC1_COMMAND, PIC_EOI);
}