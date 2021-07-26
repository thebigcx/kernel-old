#include <arch/x86_64/system.h>

void breakpoint()
{
    asm volatile ("1: jmp 1b");
}
