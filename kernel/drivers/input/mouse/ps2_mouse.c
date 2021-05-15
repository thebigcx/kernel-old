#include <drivers/input/mouse/ps2_mouse.h>
#include <io.h>
#include <gdt/idt.h>
#include <stdio.h>

static void mouse_wait()
{
    uint64_t time = 100000;
    while (time--)
    {
        if ((inb(0x64) & 0b10) == 0)
            return;
    }
}

static void mouse_wait_input()
{
    uint64_t time = 100000;
    while (time--)
    {
        if (inb(0x64) & 0b10)
            return;
    }
}

static uint8_t mouse_read()
{
    mouse_wait_input();
    return inb(0x60);
}

static void mouse_write(uint8_t value)
{
    mouse_wait();
    outb(0x64, 0xd4);
    mouse_wait();
    outb(0x60, value);
}

void mouse_handler()
{
    puts("Mouse\n");
}

void mouse_start()
{
    outb(0x64, 0xa8);
    mouse_wait();

    outb(0x64, 0x20);
    mouse_wait_input();
    uint8_t status = inb(0x60);
    status |= 0b10;
    mouse_wait();
    outb(0x64, 0x60);
    mouse_wait();
    outb(0x60, status);

    mouse_write(0xf6);
    mouse_read();

    mouse_write(0xf4);
    mouse_read();
}

void mouse_init()
{
    idt_set_irq(12, mouse_handler);
}