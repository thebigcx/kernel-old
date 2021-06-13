#include <system.h>
#include <stdio.h>
#include <console.h>

void panic(const char* msg, reg_ctx_t* regs)
{
    console_clear();

    char buffer[100];

    console_write("Kernel Panic: \n", 255, 0, 0);
    console_write("\n", 0, 0, 0);
    console_write(msg, 255, 0, 0);
    console_write("\n\n", 0, 0, 0);

    console_write("RAX = ", 255, 255, 255);
    console_write(itoa(regs->rax, buffer, 16), 255, 255, 255);
    console_write("\n", 0, 0, 0);

    console_write("RBX = ", 255, 255, 255);
    console_write(itoa(regs->rbx, buffer, 16), 255, 255, 255);
    console_write("\n", 0, 0, 0);

    console_write("RCX = ", 255, 255, 255);
    console_write(itoa(regs->rcx, buffer, 16), 255, 255, 255);
    console_write("\n", 0, 0, 0);

    console_write("RDX = ", 255, 255, 255);
    console_write(itoa(regs->rdx, buffer, 16), 255, 255, 255);
    console_write("\n", 0, 0, 0);

    console_write("RIP = ", 255, 255, 255);
    console_write(itoa(regs->rip, buffer, 16), 255, 255, 255);
    console_write("\n", 0, 0, 0);

    asm ("1: jmp 1b");
}