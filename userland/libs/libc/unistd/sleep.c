#include <unistd.h>
#include <sys/syscall.h>

void sleepns(uint64_t ns)
{
    syscall(SYS_SLEEPNS, ns);
}

void sleepus(uint64_t us)
{
    sleepns(us * 1000);
}

void sleepms(uint64_t ms)
{
    sleepns(ms * 1000000);
}

void sleeps(uint64_t s)
{
    sleepns(s * 1000000000);
}