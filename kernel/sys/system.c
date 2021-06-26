#include <sys/system.h>

void breakpoint()
{
    asm volatile ("1: jmp 1b");
}