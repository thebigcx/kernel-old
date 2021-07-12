// Stack smash protector
#include <util/types.h>
#include <drivers/tty//serial/serial.h>

#define STACK_CHK_GUARD 0x595e9fbd94fda766

uint64_t __stack_chk_guard = STACK_CHK_GUARD;

__attribute__((noreturn)) void __stack_chk_fail()
{
    serial_writestr("Stack smashing detected\n");

    asm volatile ("1: jmp 1b");
}