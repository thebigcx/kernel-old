#include <drivers/input/keyboard/ps2kb.h>
#include <intr/idt.h>


#include <sys/io.h>
#include <sys/system.h>
#include <intr/apic.h>

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
}

void kb_init()
{
    idt_set_int(33, kb_interrupt_handler);
    ioapic_map_irq(1);

    key_queue.queue_end = -1;

    vfs_node_t* node = kmalloc(sizeof(vfs_node_t));
    node->read = kb_read;
    node->flags = FS_BLKDEV;
    node->name = strdup("keyboard");
    vfs_mount(node, "/dev/keyboard");
}

bool kb_get_key(uint32_t* key)
{
    if (key_queue.queue_end < 0)
        return false;
    
    *key = key_queue.keys[key_queue.queue_end--];

    return true;
}

size_t kb_read(vfs_node_t* file, void* ptr, size_t off, size_t size)
{
    size_t i = 0;
    for (; i < size; i++)
    {
        if (!kb_get_key(ptr++))
            break;
    }

    return i;
}