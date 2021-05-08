#include <io.h>

void outb(uint16_t port, uint8_t value)
{
    asm volatile ("outb %0, %1"::"a"(value), "Nd"(port));
}

void outw(uint16_t port, uint16_t value)
{
    asm volatile ("outw %0, %1"::"a"(value), "Nd"(port));
}

void outl(uint16_t port, uint32_t value)
{
    asm volatile ("outl %0, %1"::"a"(value), "Nd"(port));
}

uint8_t inb(uint16_t port)
{
    uint8_t v;
    asm volatile ("inb %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

uint16_t inw(uint16_t port)
{
    uint16_t v;
    asm volatile ("inw %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

uint32_t inl(uint16_t port)
{
    uint32_t v;
    asm volatile ("inl %1, %0" : "=a"(v) : "Nd"(port));
    return v;
}

void outsb(uint16_t port, const void* addr, uint64_t count)
{
    asm volatile ("cld ; rep ; outsb":"=S" (addr), "=c" (count):"d" (port), "0" (addr), "1" (count));
}

void outsw(uint16_t port, const void* addr, uint64_t count)
{
    asm volatile ("cld ; rep ; outsw":"=S" (addr), "=c" (count):"d" (port), "0" (addr), "1" (count));
}

void outsl(uint16_t port, const void* addr, uint64_t count)
{
    asm volatile ("cld ; rep ; outsl":"=S" (addr), "=c" (count):"d" (port), "0" (addr), "1" (count));
}

void insb(uint16_t port, void* addr, uint64_t count)
{
    asm volatile ("cld ; rep ; insb":"=D" (addr), "=c" (count):"d" (port), "0" (addr), "1" (count));
}

void insw(uint16_t port, void* addr, uint64_t count)
{
    asm volatile ("cld ; rep ; insw":"=D" (addr), "=c" (count):"d" (port), "0" (addr), "1" (count));
}

void insl(uint16_t port, void* addr, uint64_t count)
{
    asm volatile ("cld ; rep ; insl":"=D" (addr), "=c" (count):"d" (port), "0" (addr), "1" (count));
}

void iowait()
{
    asm volatile ("outb %%al, $0x80" :: "a"(0));
}