#pragma once

#include <util/types.h>
#include <sys/system.h>

#define PIT_FREQ 1193182

#define PIT_CHAN0       0x40
#define PIT_CHAN1       0x41
#define PIT_CHAN2       0x42
#define PIT_CMD         0x43

#define PIT_CMD_BIN     0x00
#define PIT_CMD_BCD     0x01

#define PIT_CMD_MODE0   0x00
#define PIT_CMD_MODE1   0x02
#define PIT_CMD_MODE2   0x04
#define PIT_CMD_MODE3   0x06
#define PIT_CMD_MODE4   0x08
#define PIT_CMD_MODE5   0x0a

#define PIT_CMD_LATCH   0x00
#define PIT_CMD_RW_LOW  0x10
#define PIT_CMD_RW_HI   0x20
#define PIT_CMD_RW_BOTH 0x30

#define PIT_CMD_CHAN0   0x00
#define PIT_CMD_CHAN1   0x40
#define PIT_CMD_CHAN2   0x80
#define PIT_CMD_RBACK   0xc0

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

// pit.c
void pit_init(uint64_t frequency);
time_t pit_uptime(); // Time (in microseconds) since boot
void pit_handler(reg_ctx_t* r);
void pit_wait(uint64_t us); // Wait for microseconds
void pit_waitms(uint64_t ms); // Wait for milliseconds

// time.c
timept_t mk_timept(time_t time);