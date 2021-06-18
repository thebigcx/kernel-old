#include <sched/sched.h>
#include <time/time.h>

proc_t* sleep_tsk_lst;

static void nano_sleep_until(uint64_t t)
{
    cli();

    if (t < pit_boot_time() * 1000)
    {
        sti();
        return;
    }

    last_proc->sleep_exp = t;
    last_proc->next = sleep_tsk_lst;
    sleep_tsk_lst = last_proc;

    sti();

    sched_block(PROC_STATE_SLEEP);
}

void nano_sleep(uint64_t ns)
{
    nano_sleep_until(pit_boot_time() * 1000 + ns);
}

void sleep(uint64_t s)
{
    nano_sleep(s * 1000000000);
}