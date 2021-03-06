#include <drivers/tty//serial/serial.h>
#include <sys/io.h>
#include <util/stdlib.h>
#include <util/spinlock.h>

lock_t serial_lock = 0;

void serial_init()
{
    outb(SERIAL_INTR, 0x0);
    outb(SERIAL_LINECTRL, SERIAL_SETBAUD);
    outb(SERIAL_DIVLO, 0x04);
    outb(SERIAL_DIVHI, 0x00);
    outb(SERIAL_LINECTRL, SERIAL_CHARLEN8);
    outb(SERIAL_FIFO, 0xc7);
    outb(SERIAL_MODEMCTRL, 0x0b);
    outb(SERIAL_MODEMCTRL, 0x1e);
    
    // Test
    outb(PORT_COM1, 0xae);
    if (inb(PORT_COM1) != 0xae)
        return; // Faulty

    outb(SERIAL_MODEMCTRL, 0x0f);
}

bool serial_received()
{
    return inb(SERIAL_LINESTAT) & 0x01;
}

char serial_read()
{
    while (serial_received() == 0); // Spin
    return inb(PORT_COM1);
}

bool serial_transmit_empty()
{
    return inb(SERIAL_LINESTAT) & 0x20;
}

void serial_write(char c)
{
    while (serial_transmit_empty() == 0); // Spin
    outb(PORT_COM1, c);
}

void serial_writestr(char* str)
{
	if (!str) return;

    acquire_lock(serial_lock);

    while (*str != 0)
    {
        serial_write(*str++);
    }
	
    release_lock(serial_lock);
}

void serial_printf(char* format, ...)
{
    va_list list;
    va_start(list, format);
    char str[100];
    vsnprintf(str, format, list);
    va_end(list);
    serial_writestr(str);
}
