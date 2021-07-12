#include <arch/x86_64/pit.h>
#include <sys/io.h>
#include <sched/sched.h>
#include <intr/apic.h>
#include <time/time.h>

volatile uint64_t uptime;
uint64_t freq;
//timer_t timer;

void pit_int_handler(reg_ctx_t* r)
{
    uptime += (uint64_t)1000000000 / freq;
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

    //timer_t* timer = kmalloc(sizeof(timer_t) + 16);
    //timer.uptime = pit_uptime;
    //timer.wait = pit_wait;
    //timer_init(&timer);
}

uint64_t pit_uptime()
{
    return uptime;
}

void pit_wait(uint64_t ns)
{
    uint64_t now = uptime;
    while (uptime - now < ns);
}