#include <io.h>

void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %0, %1"::"a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
    uint8_t v;
    asm volatile ("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

void iowait()
{
    asm volatile ("outb %%al, $0x80" :: "a"(0));
}