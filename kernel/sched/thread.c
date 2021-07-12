#include <sched/thread.h>
#include <cpu/cpu.h>
#include <time/time.h>

void thread_block(int state)
{
    cpu_t* cpu = cpu_getcurr();

    acquire_lock(cpu->lock);
    cpu->currthread->state = state;
    release_lock(cpu->lock);

    asm volatile ("int $0x80");
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
        asm volatile ("int $0x80");
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
    
}