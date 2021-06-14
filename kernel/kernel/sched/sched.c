#include <sched/sched.h>
#include <paging/paging.h>
#include <gdt/idt.h>
#include <drivers/input/keyboard/ps2_keyboard.h>
#include <drivers/input/mouse/ps2_mouse.h>
#include <elf.h>
#include <drivers/fs/vfs/vfs.h>
#include <types.h>
#include <stdlib.h>
#include <mem/heap.h>

proc_t* ready_lst_start;
proc_t* ready_lst_end;
proc_t* last_proc;

uint32_t post_tsk_cnt = 0;
uint32_t tsk_post_flag = 0;

void sched_init()
{
    asm ("sti");
    for (;;);
}

void schedule(reg_ctx_t* r)
{
    if (post_tsk_cnt != 0)
    {
        tsk_post_flag = 1;
        return;
    }

    if (ready_lst_start != NULL)
    {
        last_proc->regs = *r;
        last_proc->state = PROC_STATE_READY;

        proc_t* task = ready_lst_start;
        ready_lst_start = task->next;
        last_proc = task;
        task->state = PROC_STATE_RUNNING;

        task_switch(&(task->regs), (uint64_t)task->addr_space);
    }
    else if (last_proc->state == PROC_STATE_RUNNING)
    {
        // Let it continue running
    }
    else
    {

    }
}

proc_t* mk_proc(void* entry)
{
    proc_t* proc = kmalloc(sizeof(proc_t));
    proc->state = PROC_STATE_READY;
    proc->addr_space = page_get_kpml4();
    proc->pid = 0;
    proc->next = NULL;

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
    schedule(r);
}

void sched_spawn_proc(proc_t* proc)
{
    sched_lock();

    if (!ready_lst_start || !ready_lst_end) // First process
    {
        ready_lst_start = proc;
        ready_lst_end = proc;
        return;
    }

    proc_t* last = ready_lst_end;
    last->next = proc;
    ready_lst_end = proc;

    sched_unlock();
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
    sched_lock();

    last_proc->state = state;
    //schedule(&(last_proc->regs));

    sched_unlock();
}

void sched_unblock(proc_t* proc)
{
    sched_lock();

    if (ready_lst_start == NULL)
    {
        // Switch straight to it
        task_switch(&(proc->regs), proc->addr_space);
    }
    else
    {
        // Need to wait (other processes running)
        ready_lst_end->next = proc;
        ready_lst_end = proc;
        proc->state = PROC_STATE_READY;
    }

    sched_unlock();
}

int irq_disable_cnt = 0;

void sched_lock()
{
    asm ("cli");
    irq_disable_cnt++;
}

void sched_unlock()
{
    irq_disable_cnt--;
    if (irq_disable_cnt == 0)
    {
        asm ("sti");
    }
}

void sched_lock_stuff()
{
    asm ("cli");
    irq_disable_cnt++;
    post_tsk_cnt++;
}

void sched_unlock_stuff()
{
    post_tsk_cnt--;
    if (post_tsk_cnt == 0)
    {
        if (tsk_post_flag != 0)
        {
            tsk_post_flag = 0;
            //schedule();
        }
    }

    irq_disable_cnt--;
    if (irq_disable_cnt == 0)
    {
        asm ("sti");
    }
}