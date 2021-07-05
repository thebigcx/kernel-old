#include <sys/system.h>
#include <drivers/tty/serial.h>

void panic(const char* msg)
{
    cli();

    serial_writestr("Kernel Panic: \n");
    serial_writestr(msg);

    asm volatile ("1: jmp 1b");
}