#include <system.h>
#include <stdio.h>

void panic(const char* msg)
{
    puts("Kernel Panic: ");
    puts(msg);

    while (1);
}