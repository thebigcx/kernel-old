#pragma once

#include <stdint.h>
#include <system.h>

#define PIT_FREQ 1193182

#define PIT_CHANNEL0 0x40
#define PIT_CHANNEL1 0x41
#define PIT_CHANNEL2 0x42
#define PIT_COMMAND  0x43

typedef uint64_t time_t;

// pit.c
void pit_init(uint64_t frequency);
time_t pit_boot_time(); // Time (in microseconds) since boot
void pit_handler(reg_ctx_t* r);

// time.c
