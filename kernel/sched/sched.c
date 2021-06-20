#include <sched/sched.h>
#include <mem/paging.h>
#include <intr/idt.h>
#include <drivers/input/keyboard/ps2_keyboard.h>
#include <drivers/input/mouse/ps2_mouse.h>
#include <util/elf.h>
#include <drivers/fs/vfs/vfs.h>
#include <util/types.h>
#include <util/stdlib.h>
#include <mem/heap.h>
#include <sys/system.h>
#include <sched/spinlock.h>

void ctx_switch(reg_ctx_t* regs, uint64_t pml4);

proc_t* ready_lst_start;
proc_t* ready_lst_end;
lock_t ready_lst_lock = 0;
proc_t* last_proc;

proc_t* idle_proc;

int scheduler_ready = 0;

void idle()
{
    while (1);
    //hlt();
}

void sched_init()
{
    idt_set_int(IPI_SCHED, schedule); // Software interrupt for schedule (0xfd)

    idle_proc = mk_proc(idle);

    scheduler_ready = 1;

    sti();
    for (;;);
}

void schedule(reg_ctx_t* r)
{
    proc_t* tsk = ready_lst_start;

    //if (!tsk) tsk = idle_proc;

    if (tsk != NULL)
    {
        last_proc->regs = *r;
        if (last_proc->state == PROC_STATE_RUNNING)
            last_proc->state = PROC_STATE_READY;

        ready_lst_start = tsk->next;
        last_proc = tsk;
        tsk->state = PROC_STATE_RUNNING;

        ctx_switch(&(tsk->regs), (uint64_t)tsk->addr_space);
    }
    //else if (last_proc->state == PROC_STATE_RUNNING)
    {
        // Let it continue running
    }
    /*else
    {
        proc_t* tsk = last_proc;
        last_proc = NULL;
    
        do
        {
            hlt();
        } while (ready_lst_start == NULL); // Wait for task

        last_proc = tsk;
        tsk = ready_lst_start;
        if (tsk != last_proc)
        {
            ctx_switch(&(tsk->regs), (uint64_t)tsk->addr_space);
        }
    }*/
}

proc_t* mk_proc(void* entry)
{
    proc_t* proc = kmalloc(sizeof(proc_t));
    proc->state = PROC_STATE_READY;
    proc->addr_space = page_get_kpml4();
    proc->pid = 0;
    proc->next = NULL;
    proc->sleep_exp = 0;

    void* stack = kmalloc(1000);
    memset(stack, 0, 1000);

    memset(&(proc->regs), 0, sizeof(reg_ctx_t));
    proc->regs.rip = (uint64_t)entry;
    proc->regs.rflags = 0x202;
    proc->regs.cs = KERNEL_CS;
    proc->regs.ss = KERNEL_SS;
    proc->regs.rbp = (uint64_t)stack + 1000;
    proc->regs.rsp = (uint64_t)stack + 1000;

    return proc;
}

void sched_tick(reg_ctx_t* r)
{
    if (!scheduler_ready) return;

    cli();

    proc_t* next = sleep_tsk_lst;
    proc_t* this;
    sleep_tsk_lst = NULL;

    while (next != NULL)
    {
        this = next;
        next = this->next;

        if (this->sleep_exp <= pit_uptime() * 1000)
        {
            sched_unblock(this);
        }
        else
        {
            this->next = sleep_tsk_lst;
            sleep_tsk_lst = this;
        }
    }

    sti();

    schedule(r);
}

void sched_spawn_proc(proc_t* proc)
{
    //acquire_lock(ready_lst_lock);

    if (!ready_lst_start || !ready_lst_end) // First process
    {
        ready_lst_start = proc;
        ready_lst_end = proc;
        //proc->next = proc;
        return;
    }

    proc_t* last = ready_lst_end;
    last->next = proc;
    ready_lst_end = proc;
    //proc->next = ready_lst_start;

    //release_lock(ready_lst_lock);
}

void sched_kill_proc()
{
    /*sched_lock();

    last_proc->next = kill_tsk_lst;
    kill_tsk_lst = last_proc;

    sched_unlock();

    sched_block(PROC_STATE_KILLED);

    sched_unblock();*/
}

void sched_block(uint32_t state)
{
    cli();

    last_proc->state = state;
    asm ("int $0xfd"); // Send IPI schedule

    sti();
}

void sched_unblock(proc_t* proc)
{
    cli();

    if (ready_lst_start)
    {
        // Need to wait (other processes running)
        ready_lst_end->next = proc;
        ready_lst_end = proc;
        proc->state = PROC_STATE_READY;
    }
    else
    {
        ready_lst_start = proc;
        ready_lst_end = proc;
    }

    sti();
}