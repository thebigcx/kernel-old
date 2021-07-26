#pragma once

#include <util/types.h>
#include <arch/x86_64/system.h>

typedef uint64_t time_t;
// Time point
typedef struct timept
{
    uint64_t years;
    uint16_t days;
    uint8_t hours;
    uint8_t mins;
    uint8_t secs;
    uint8_t ms;
    uint8_t us;

} timept_t;

typedef struct timer
{
	uint64_t (*uptime)();
	void (*wait)(uint64_t ns);

} timer_t;

uint64_t timer_uptime();
void timer_wait(uint64_t ns);
