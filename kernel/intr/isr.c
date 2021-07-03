#include <intr/idt.h>
#include <drivers/tty/serial.h>

#define DUMPREG(reg, val)\
    serial_writestr(reg"=");\
    serial_writestr(ultoa(val, buffer, 16));\
    serial_writestr("\n");

void dumpregs(reg_ctx_t* r)
{
    char buffer[100];

    DUMPREG("RAX",      r->rax);
    DUMPREG("RBX",      r->rbx);
    DUMPREG("RCX",      r->rcx);
    DUMPREG("RDX",      r->rdx);
    DUMPREG("RIP",      r->rip);
    DUMPREG("RDI",      r->rdi);
    DUMPREG("RSI",      r->rsi);
    DUMPREG("RSP",      r->rsp);
    DUMPREG("RBP",      r->rbp);
    DUMPREG("CS",       r->cs);
    DUMPREG("SS",       r->ss);
    DUMPREG("RFLAGS",   r->rflags);
}

void generic_isr(const char* err, isr_frame_t* frame)
{
    serial_writestr(err);
    dumpregs(&frame->regs);

    asm volatile ("1: jmp 1b");
}

void pagefault_handler(isr_frame_t* frame)
{
    serial_writestr("Page fault\n");
    dumpregs(&frame->regs);

    if (frame->errcode & (1 << 4))
        serial_writestr("Invalid instruction fetch\n");
    if (frame->errcode & (1 << 3))
        serial_writestr("Reserved bits set\n");
    if (frame->errcode & (1 << 2))
        serial_writestr("CPL = 3\n");

    if (frame->errcode & (1 << 1))
        serial_writestr("Invalid write\n");
    else
        serial_writestr("Invalid read\n");

    if (frame->errcode & (1 << 0))
        serial_writestr("Page protection violation\n");
    else
        serial_writestr("Non-present page\n");

    asm volatile ("1: jmp 1b");
}

void general_protection_fault_handler(isr_frame_t* frame)
{
    serial_writestr("General protection fault\n");
    dumpregs(&frame->regs);

    asm volatile ("1: jmp 1b");
}