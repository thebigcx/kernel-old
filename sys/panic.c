#include <arch/x86_64/system.h>
#include <drivers/tty/serial/serial.h>

void panic(const char* msg)
{
    cli();

    serial_writestr("Kernel Panic: \n");
    serial_writestr(msg);

    asm volatile ("1: jmp 1b");
}
