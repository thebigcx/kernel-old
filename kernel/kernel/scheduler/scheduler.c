#include <scheduler/scheduler.h>
#include <stddef.h>
#include <paging/paging.h>

proc_t* ready_lst_start;
proc_t* ready_lst_end;
int irq_disable_cnt = 0;

extern void kernel_proc();

void scheduler_init()
{
    proc_t* kproc = mk_proc(kernel_proc);
    ready_lst_start = kproc;
    ready_lst_end = kproc;
    schedule();
    for (;;);
}

void schedule()
{
    if (ready_lst_start != NULL)
    {
        proc_t* task = ready_lst_start;
        ready_lst_start = task->next;
        task_switch(&(task->regs), task->addr_space);
    }
}

void scheduler_lock()
{
    asm volatile ("cli");
    irq_disable_cnt++;
}

void scheduler_unlock()
{
    irq_disable_cnt--;
    if (irq_disable_cnt == 0)
        asm volatile ("sti");
}

proc_t* mk_proc(void* entry)
{
    proc_t* proc = malloc(sizeof(proc_t));
    proc->state = PROC_STATE_READY;
    proc->addr_space = page_get_pml4();
    proc->pid = 0;
    proc->next = NULL;

    memset(&(proc->regs), 0, sizeof(reg_ctx_t));
    proc->regs.rip = entry;
    proc->regs.rflags = 0x202;
    proc->regs.cs = KERNEL_CS;
    proc->regs.ss = KERNEL_SS;

    void* stack = page_request();
    page_map_memory(stack, stack);
    for (int i = 0; i < 31; i++)
    {
        page_map_memory(page_request(), stack + i * PAGE_SIZE + 1);
    }

    proc->regs.rbp = (uint64_t)stack + PAGE_SIZE * 32;
    proc->regs.rsp = (uint64_t)stack + PAGE_SIZE * 32;

    return proc;
}