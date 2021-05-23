#include <drivers/input/mouse/ps2_mouse.h>
#include <io.h>
#include <gdt/idt.h>
#include <stdio.h>

int8_t mouse_data[3];
uint8_t cycle = 0;

#define MOUSE_QUEUE_SIZE 512

struct
{
    mouse_packet_t packets[MOUSE_QUEUE_SIZE];
    int32_t packet_end; // -1 when no packets

} mouse_queue;

static void mouse_wait()
{
    uint64_t time = 100000;
    while (time--)
    {
        if ((inb(0x64) & 2) == 0)
            return;
    }
}

static void mouse_wait_input()
{
    uint64_t time = 100000;
    while (time--)
    {
        if ((inb(0x64) & 1) == 1)
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
    switch (cycle)
    {
        case 0:
            mouse_data[0] = inb(0x60);

            if (!(mouse_data[0] & 0x8)) break;

            cycle++;
            break;

        case 1:
            mouse_data[1] = inb(0x60);
            cycle++;
            break;

        case 2:
            mouse_data[2] = inb(0x60);
            cycle = 0;

            mouse_packet_t pack;
            pack.buttons = mouse_data[0] & (MOUSE_BTN_LEFT | MOUSE_BTN_RIGHT | MOUSE_BTN_MID);

            pack.x_mov = mouse_data[1];
            pack.y_mov = -mouse_data[2];

            mouse_queue.packets[++mouse_queue.packet_end] = pack;
            if (mouse_queue.packet_end > MOUSE_QUEUE_SIZE)
            {
                mouse_queue.packet_end = -1;
            }

            break;

        default:
            cycle = 0;
            break;
    }
}

void mouse_map_int()
{
    idt_set_irq(12, mouse_handler);
}

void mouse_init()
{
    mouse_queue.packet_end = -1;

    mouse_wait();
    outb(0x64, 0xa8);

    outb(0x64, 0xff);
    mouse_wait();

    outb(0x64, 0x20);
    mouse_wait_input();
    uint8_t status = ((inb(0x60) & ~0x20) | 2);
    mouse_wait();
    outb(0x64, 0x60);
    mouse_wait();
    outb(0x60, status);

    mouse_write(0xf6);
    mouse_read();

    mouse_write(0xf4);
    mouse_read();
}

bool mouse_get_packet(mouse_packet_t* packet)
{
    if (mouse_queue.packet_end < 0)
        return false;

    *packet = mouse_queue.packets[mouse_queue.packet_end--];

    return true;
}