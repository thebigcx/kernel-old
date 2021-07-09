#include <time/time.h>
#include <intr/idt.h>

#include <sys/io.h>
#include <sched/sched.h>
#include <intr/apic.h>

volatile uint64_t uptime;
uint64_t freq;

void pit_int_handler(reg_ctx_t* r)
{
    uptime += 1000000 / freq;
    sched_tick(r);
}

void pit_init(uint64_t frequency)
{
    idt_set_int(32, pit_int_handler);

    freq = frequency;
    uint32_t div = PIT_FREQ / freq;

    outb(PIT_CMD, PIT_CMD_BIN | PIT_CMD_MODE3 | PIT_CMD_RW_BOTH | PIT_CMD_CHAN0);

    outb(PIT_CHAN0, div & 0xff);
    outb(PIT_CHAN0, div >> 8);
}

time_t pit_uptime()
{
    return uptime;
}

void pit_wait(uint64_t us)
{
    uint64_t now = uptime;
    while (uptime - now < us);
}

void pit_waitms(uint64_t ms)
{
    pit_wait(ms * 1000);
}