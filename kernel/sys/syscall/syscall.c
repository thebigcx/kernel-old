#include <sys/syscall/syscall.h>
#include <sys/console.h>
#include <mem/paging.h>

#define SYSCALL_CNT 100

uint64_t sys_open(reg_ctx_t* regs)
{
    console_write("Syscall open!", 255, 255, 255);
}

uint64_t sys_read(reg_ctx_t* regs)
{
    console_write("Syscall read!", 255, 255, 255);
}

uint64_t sys_write(reg_ctx_t* regs)
{
    console_write("Syscall write!", 255, 255, 255);
}

uint64_t sys_close(reg_ctx_t* regs)
{
    console_write("Syscall close!", 255, 255, 255);
}

syscall_t syscalls[SYSCALL_CNT] =
{
    sys_open,
    sys_read,
    sys_write,
    sys_close
};

void syscall_handler(reg_ctx_t* regs)
{
    

    regs->rax = syscalls[regs->rax](regs);
}