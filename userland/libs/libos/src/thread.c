#include <os/thread.h>
#include <os/syscall.h>

int thread_creat(thread_t* thread, void* entry, void* arg)
{
    return syscall(SYS_THREADCREAT, thread, entry, arg);
}

void thread_exit()
{
    syscall(SYS_THREADEXIT);
}

void thread_sleepns(uint64_t ns)
{
    syscall(SYS_SLEEPNS, ns);
}

void thread_sleepus(uint64_t us)
{
    thread_sleepns(us * 1000);
}

void thread_sleepms(uint64_t ms)
{
    thread_sleepns(ms * 1000000);
}

void thread_sleeps(uint64_t s)
{
    thread_sleepns(s * 1000000000);
}