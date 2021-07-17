#include <sched/thread.h>
#include <cpu/cpu.h>
#include <time/time.h>
#include <cpu/smp.h>
#include <sched/sched.h>

void thread_spawn(thread_t* thread)
{
    cli();

    // Find the CPU with the least threads to add the new one to
    cpu_t* cpu = &cpus[0];
    for (uint32_t i = 1; i < cpu_count; i++)
    {
        cpu_t* nextcpu = &cpus[i];

        if (nextcpu->threads->cnt < cpu->threads->cnt)
        {
            cpu = nextcpu;
        }
    }

    acquire_lock(cpu->lock);
    list_push_back(cpu->threads, thread);
    list_push_back(cpu->run_queue, thread);
    release_lock(cpu->lock);

    sti();
}

thread_t* thread_creat(proc_t* parent, void* entry, int kernel)
{
    thread_t* thread = kmalloc(sizeof(thread_t));
    thread->parent = parent;
    thread->sleepexp = 0;
    thread->state = THREAD_STATE_READY;
    thread->tid = parent->nexttid++;

    memset(&thread->regs, 0, sizeof(reg_ctx_t));
    thread->regs.cs = kernel ? KERNEL_CS : USER_SS;
    thread->regs.ss = kernel ? KERNEL_SS : USER_SS;
    thread->regs.rflags = RFLAG_INTR | 0x2;
    thread->regs.rip = (uint64_t)entry;
    // TODO: thread stack

    return thread;
}

void thread_block(int state)
{
    cpu_t* cpu = cpu_getcurr();

    acquire_lock(cpu->lock);
    cpu->currthread->state = state;
    release_lock(cpu->lock);

    asm volatile ("int $0xfd");
}

void thread_unblock(thread_t* thread)
{
    cpu_t* cpu = cpu_getcurr();
    acquire_lock(cpu->lock);
    thread->state = THREAD_STATE_RUNNING;
    list_push_back(cpu->run_queue, thread);
    release_lock(cpu->lock);

    if (cpu->run_queue->cnt == 1)
    {
        // No point wainting around for the next tick, schedule immediately
        asm volatile ("int $0xfd");
    }
}

void thread_sleepuntil(uint64_t timepoint)
{
    //if (timepoint <= timer_uptime())
    if (timepoint <= pit_uptime())
        return;

    cpu_t* cpu = cpu_getcurr();
    acquire_lock(cpu->lock);
    cpu->currthread->sleepexp = timepoint;
    release_lock(cpu->lock);
    thread_block(THREAD_STATE_ASLEEP);
}

void thread_sleepns(uint64_t ns)
{
    //thread_sleepuntil(timer_uptime() + ns);
    thread_sleepuntil(pit_uptime() + ns);
}

void thread_exit()
{
    thread_block(THREAD_STATE_KILLED);
}

void thread_kill(thread_t* thread)
{
    thread->state = THREAD_STATE_KILLED;
}