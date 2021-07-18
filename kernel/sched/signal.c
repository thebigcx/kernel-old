#include <sched/sched.h>

void signal_send(proc_t* proc, int signal)
{
    signal_t* sig = kmalloc(sizeof(signal_t));
    sig->signo = signal;
    sig->handler = proc->signals[signal];
    list_push_back(proc->sigstack, sig);

    // Only handled once schedule() is called again
    if (proc == sched_get_currproc())
    {
        sched_yield();
    }
}