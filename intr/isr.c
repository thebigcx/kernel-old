#include <intr/idt.h>
#include <drivers/tty//serial/serial.h>
#include <sched/sched.h>
#include <intr/apic.h>
#include <util/spinlock.h>

#define DUMPREG(reg, val)\
    serial_writestr(reg"=");\
    serial_writestr(ultoa(val, buffer, 16));\
    serial_writestr("\n");

lock_t isrlock = 0;

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
    if (frame->regs.ss & 0x3)
    {
        proc_t* proc = sched_get_currproc();
        serial_printf("Process (PID %d) crashed: %s\n", proc->pid, err);
        serial_printf("Faulting address: 0x%x\n", frame->regs.rip);
        sched_kill(proc);
    }
    else
    {
        lapic_send_ipi(0, ICR_ALL_EX_SELF, ICR_FIXED, IPI_HALT);
        serial_writestr(ANSI_RED);
        serial_writestr("Kernel Panic\n\n");
        serial_writestr(err);
        dumpregs(&frame->regs);

        asm volatile ("1: jmp 1b");
    }
}

void pagefault_handler(isr_frame_t* frame)
{
	acquire_lock(isrlock);
    if (frame->regs.ss & 0x3)
    {
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
		
		uint64_t cr2;
        asm volatile ("mov %%cr2, %0" : "=r"(cr2));
        serial_printf("Faulting address: %x\n", cr2);
        
		release_lock(isrlock);
		proc_t* proc = sched_get_currproc();
        signal_send(proc, SIGSEG);
    }
    else
    {
        lapic_send_ipi(0, ICR_ALL_EX_SELF, ICR_FIXED, IPI_HALT);
        serial_writestr(ANSI_RED);
        serial_writestr("Kernel Panic\n\n");
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

        uint64_t cr2;
        asm volatile ("mov %%cr2, %0" : "=r"(cr2));
        serial_printf("Faulting address: %x\n", cr2);

		release_lock(isrlock);
        asm volatile ("1: jmp 1b");
    }
}

void general_protection_fault_handler(isr_frame_t* frame)
{
	acquire_lock(isrlock);
    if (frame->regs.ss & 0x3)
    {
		release_lock(isrlock);
        proc_t* proc = sched_get_currproc();
        signal_send(proc, SIGSEG);
    }
    else
    {
        lapic_send_ipi(0, ICR_ALL_EX_SELF, ICR_FIXED, IPI_HALT);
        serial_writestr(ANSI_RED);
        serial_writestr("Kernel Panic\n\n");
        serial_writestr("General protection fault\n");
        dumpregs(&frame->regs);

        if (frame->errcode)
        {
            serial_writestr("\nError code:\n\n");

            if (frame->errcode & 0x1)
            {
                serial_writestr("External\n");
            }
            switch (frame->errcode & 0x6)
            {
                case 0x0:
                    serial_writestr("GDT selector index: ");
                    break;
                case 0x1:
                    serial_writestr("IDT selector index: ");
                    break;
                case 0x2:
                    serial_writestr("LDT selector index: ");
                    break;
                case 0x3:
                    serial_writestr("IDT selector index: ");
                    break;
            }

            serial_printf("%d\n", (frame->errcode >> 3) & 13);
        }
        else
        {
            serial_writestr("No error code");
        }

		release_lock(isrlock);
        asm volatile ("1: jmp 1b");
    }
}
