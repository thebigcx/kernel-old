#include <system.h>

#include <console.h>

#define REG_OUT(name, reg)\
    console_write(name" = ", 255, 255, 255);\
    console_write(itoa(reg, buffer, 16), 255, 255, 255);\
    console_write("\n", 0, 0, 0);

void panic(const char* msg, reg_ctx_t* regs)
{
    //console_clear();

    char buffer[100];

    console_write("Kernel Panic: ", 255, 0, 0);
    console_write(msg, 255, 0, 0);
    console_write("\n\n", 0, 0, 0);

    REG_OUT("RAX", regs->rax);
    REG_OUT("RBX", regs->rbx);
    REG_OUT("RCX", regs->rcx);
    REG_OUT("RDX", regs->rdx);
    REG_OUT("RIP", regs->rip);
    REG_OUT("RDI", regs->rdi);
    REG_OUT("RSI", regs->rsi);
    REG_OUT("RSP", regs->rsp);
    REG_OUT("RBP", regs->rbp);
    REG_OUT("CS", regs->cs);
    REG_OUT("SS", regs->ss);
    REG_OUT("RFLAGS", regs->rflags);

    asm ("1: jmp 1b");
}