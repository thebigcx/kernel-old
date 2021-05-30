#pragma once

#include <stdint.h>
#include <system.h>

#define PROC_STATE_RUNNING 0
#define PROC_STATE_READY   1
#define PROC_STATE_WAITING 2

#define KERNEL_CS 0x08
#define KERNEL_SS 0x10

#define USER_CS 0x1b
#define USER_SS 0x23

typedef struct proc
{
    uint64_t pid;
    void* addr_space;
    uint8_t state;
    struct proc* next;

    reg_ctx_t regs;

} proc_t;

void task_switch(reg_ctx_t* regs, uint64_t pml4);
void sched_init();
void schedule(reg_ctx_t* r);
proc_t* mk_proc(void* entry);
proc_t* mk_elf_proc(uint8_t* elf_dat);
void sched_tick(reg_ctx_t* r);
void sched_spawn_proc(proc_t* proc);