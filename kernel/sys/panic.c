#include <sys/system.h>
#include <sys/console.h>

#define REG_OUT(name, reg)\
    serial_writestr(name" = ");\
    serial_writestr(ultoa(reg, buffer, 16));\
    serial_writestr("\n")

void panic(const char* msg, uint64_t num, isr_frame_t* r)
{
    //console_clear();

    char buffer[100];

    serial_writestr("Kernel Panic: ");
    serial_writestr(msg);
    serial_writestr("\n\n");

    while (1);

    REG_OUT("RAX",      r->regs.rax);
    REG_OUT("RBX",      r->regs.rbx);
    REG_OUT("RCX",      r->regs.rcx);
    REG_OUT("RDX",      r->regs.rdx);
    REG_OUT("RIP",      r->regs.rip);
    REG_OUT("RDI",      r->regs.rdi);
    REG_OUT("RSI",      r->regs.rsi);
    REG_OUT("RSP",      r->regs.rsp);
    REG_OUT("RBP",      r->regs.rbp);
    REG_OUT("CS",       r->regs.cs);
    REG_OUT("SS",       r->regs.ss);
    REG_OUT("RFLAGS",   r->regs.rflags);

    serial_writestr("\n");

    serial_writestr("Error code: \n\n");

    if (num == 14)
    {
        if (r->errcode & (1 << 4))
            serial_writestr("Invalid instruction fetch\n");
        if (r->errcode & (1 << 3))
            serial_writestr("Reserved bits set\n");
        if (r->errcode & (1 << 2))
            serial_writestr("CPL = 3\n");

        if (r->errcode & (1 << 1))
            serial_writestr("Invalid write\n");
        else
            serial_writestr("Invalid read\n");

        if (r->errcode & (1 << 0))
            serial_writestr("Page protection violation\n");
        else
            serial_writestr("Non-present page\n");
    }
    else
    {
        console_printf("None\n", 255, 255, 255);
    }

    asm ("1: jmp 1b");
}