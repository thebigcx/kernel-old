#pragma once

#include <util/types.h>
#include <arch/x86_64/system.h>

typedef uint64_t time_t;

typedef struct timer
{
	uint64_t (*uptime)();
	void (*wait)(uint64_t ns);

} timer_t;

typedef struct timespec
{
	uint64_t tv_sec; // Seconds
	uint64_t tv_nsec; // Nanoseconds

} timespec_t;

uint64_t timer_uptime();
void timer_wait(uint64_t ns);
