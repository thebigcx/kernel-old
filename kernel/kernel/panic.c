#include <system.h>
#include <stdio.h>

void panic(const char* msg)
{
    puts("Kernel Panic: \n");
    puts("\n");
    puts(msg);

    asm ("1: jmp 1b");
}