#include <drivers/input/keyboard/ps2_keyboard.h>
#include <gdt/idt.h>
#include <stdio.h>
#include <stddef.h>
#include <io.h>
#include <system.h>

#define KEY_QUEUE_SIZE 512

struct
{
    uint32_t keys[KEY_QUEUE_SIZE];
    int32_t queue_end;
} key_queue;

void kb_interrupt_handler(reg_ctx_t* r)
{
    uint8_t scan_code = inb(0x60);

    if (key_queue.queue_end > KEY_QUEUE_SIZE - 1)
        return;

    key_queue.keys[++key_queue.queue_end] = (uint32_t)scan_code;

    outb(PIC1_COMMAND, PIC_EOI);
}

void kb_init()
{
    idt_set_irq(1, kb_interrupt_handler);

    key_queue.queue_end = -1;
}

bool kb_get_key(uint32_t* key)
{
    if (key_queue.queue_end < 0)
        return false;

    *key = key_queue.keys[key_queue.queue_end--];

    return true;
}