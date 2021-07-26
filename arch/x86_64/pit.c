#include <arch/x86_64/pit.h>
#include <time/time.h>
#include <intr/idt.h>
#include <arch/x86_64/system.h>
#include <sched/sched.h>
#include <intr/apic.h>

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

timer_t pit_timer;
volatile uint64_t uptime;
uint64_t freq;

void pit_int_handler(reg_ctx_t* r)
{
    uptime += 1000000000 / freq;
    sched_tick(r);
}

static time_t pit_uptime()
{
    return uptime;
}

static void pit_wait(uint64_t ns)
{
    uint64_t now = uptime;
    while (uptime - now < ns);
}

void pit_init(uint64_t frequency)
{
    idt_set_int(32, pit_int_handler);

    freq = frequency;
    uint32_t div = PIT_FREQ / freq;

    outb(PIT_CMD, PIT_CMD_BIN | PIT_CMD_MODE3 | PIT_CMD_RW_BOTH | PIT_CMD_CHAN0);

    outb(PIT_CHAN0, div & 0xff);
    outb(PIT_CHAN0, div >> 8);

	//timer_t* timer = kmalloc(sizeof(timer_t));
	pit_timer.uptime = pit_uptime;
	pit_timer.wait = pit_wait;
	timer_init(&pit_timer);
}
