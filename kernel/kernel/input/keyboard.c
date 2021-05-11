#include <input/keyboard.h>
#include <gdt/idt.h>
#include <stdio.h>
#include <stddef.h>
#include <io.h>

#define KEY_QUEUE_SIZE 512

struct
{
    uint32_t keys[KEY_QUEUE_SIZE];
    int32_t queue_end;
} key_queue;

void keyboard_interrupt_handler()
{
    uint8_t scan_code = inb(0x60);

    if (key_queue.queue_end > KEY_QUEUE_SIZE - 1)
        return;

    key_queue.keys[++key_queue.queue_end] = (uint32_t)scan_code;
}

void keyboard_init()
{
    idt_set_irq(1, keyboard_interrupt_handler);

    key_queue.queue_end = -1;
}

bool keyboard_get_key(uint32_t* key)
{
    if (key_queue.queue_end < 0)
        return false;

    *key = key_queue.keys[key_queue.queue_end--];

    return true;
}