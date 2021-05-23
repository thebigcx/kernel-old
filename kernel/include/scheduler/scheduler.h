#pragma once

#include <stdint.h>
#include <system.h>

#define PROC_STATE_RUNNING 0
#define PROC_STATE_READY   1
#define PROC_STATE_WAITING 2

#define KERNEL_CS 0x08
#define KERNEL_SS 0x10

typedef struct proc
{
    uint64_t pid;
    void* addr_space;
    uint8_t state;
    struct proc* next;

    reg_ctx_t regs;

} proc_t;

void task_switch(reg_ctx_t* regs, uint64_t pml4);
void scheduler_init();
void schedule();
void scheduler_lock();
void scheduler_unlock();
proc_t* mk_proc(void* entry);