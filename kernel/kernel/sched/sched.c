#include <sched/sched.h>
#include <stddef.h>
#include <paging/paging.h>
#include <gdt/idt.h>
#include <drivers/input/keyboard/ps2_keyboard.h>
#include <drivers/input/mouse/ps2_mouse.h>
#include <elf.h>
#include <drivers/fs/vfs/vfs.h>
#include <stdint.h>
#include <string.h>

proc_t* ready_lst_start;
proc_t* ready_lst_end;
proc_t* last_proc;

extern void kernel_proc();

void sched_init()
{
    asm ("sti");
    for (;;);
}

void schedule(reg_ctx_t* r)
{
    if (ready_lst_start != NULL)
    {
        last_proc->regs = *r;
        proc_t* task = ready_lst_start;
        ready_lst_start = task->next;
        last_proc = task;
        task_switch(&(task->regs), task->addr_space);
    }
}

proc_t* mk_proc(void* entry)
{
    proc_t* proc = kmalloc(sizeof(proc_t));
    proc->state = PROC_STATE_READY;
    //proc->addr_space = page_get_kpml4(); // TODO: use page_mk_map()
    proc->addr_space = page_mk_map();
    proc->pid = 0;
    proc->next = NULL;

    void* stack = kmalloc(1000);
    memset(stack, 0, 1000);

    memset(&(proc->regs), 0, sizeof(reg_ctx_t));
    proc->regs.rip = entry;
    proc->regs.rflags = 0x202;
    proc->regs.cs = KERNEL_CS;
    proc->regs.ss = KERNEL_SS;
    proc->regs.rbp = (uint64_t)stack + 1000;
    proc->regs.rsp = (uint64_t)stack + 1000;

    return proc;
}

void sched_tick(reg_ctx_t* r)
{
    schedule(r);
}

void sched_spawn_proc(proc_t* proc)
{
    if (!ready_lst_start || !ready_lst_end) // First process
    {
        ready_lst_start = proc;
        ready_lst_end = proc;
        return;
    }

    proc_t* last = ready_lst_end;
    last->next = proc;
    ready_lst_end = proc;
}

void sched_kill_proc(proc_t* proc)
{
    
}