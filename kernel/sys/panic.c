#include <sys/system.h>
#include <sys/console.h>

#define REG_OUT(name, reg)\
    console_printf(name" = %x\n", 255, 255, 255, reg)

void panic(const char* msg, uint64_t num, isr_frame_t* r)
{
    console_clear();

    char buffer[100];

    console_write("Kernel Panic: ", 255, 0, 0);
    console_write(msg, 255, 0, 0);
    console_write("\n\n", 0, 0, 0);

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

    console_write("\n", 255, 255, 255);

    console_printf("Error code: \n\n", 255, 0, 0);

    if (num == 14)
    {
        if (r->errcode & (1 << 4))
            console_printf("Invalid instruction fetch\n", 255, 255, 255);
        if (r->errcode & (1 << 3))
            console_printf("Reserved bits set\n", 255, 255, 255);
        if (r->errcode & (1 << 2))
            console_printf("CPL = 3\n", 255, 255, 255);

        if (r->errcode & (1 << 1))
            console_printf("Invalid write\n", 255, 255, 255);
        else
            console_printf("Invalid read\n", 255, 255, 255);

        if (r->errcode & (1 << 0))
            console_printf("Page protection violation\n", 255, 255, 255);
        else
            console_printf("Non-present page\n", 255, 255, 255);
    }
    else
    {
        console_printf("None\n", 255, 255, 255);
    }

    asm ("1: jmp 1b");
}