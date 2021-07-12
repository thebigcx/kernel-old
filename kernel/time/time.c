#include <time/time.h>

timer_t* timer;

void timer_init(timer_t* timerdev)
{
    timer = timerdev;
}

uint64_t timer_uptime()
{
    return timer->uptime();
}

void timer_wait(uint64_t ns)
{
    timer->wait(ns);
}

void timer_waitus(uint64_t us)
{
    timer->wait(us * 1000);
}

void timer_waitms(uint64_t ms)
{
    timer->wait(ms * 1000000);
}

void timer_waits(uint64_t s)
{
    timer->wait(s * 1000000000);
}

timept_t mk_timept(uint64_t time)
{
    
}