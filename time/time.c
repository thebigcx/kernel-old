#include <time/time.h>

static timer_t* timer;

void timer_init(timer_t* _timer)
{
	timer = _timer;
}

uint64_t timer_uptime()
{
	return timer->uptime();
}

void timer_wait(uint64_t us)
{
	timer->wait(us);
}
