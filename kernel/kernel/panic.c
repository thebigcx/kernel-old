#include <system.h>
#include <stdio.h>

void panic(const char* msg)
{
    console_write("Kernel Panic: \n", 255, 0, 0);
    console_write("\n", 0, 0, 0);
    console_write(msg, 255, 0, 0);

    asm ("1: jmp 1b");
}