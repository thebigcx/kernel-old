#pragma once

#include <util/types.h>
#include <sys/system.h>

#define PROC_STATE_RUNNING      0
#define PROC_STATE_READY        1
#define PROC_STATE_WAITING      2
#define PROC_STATE_WAIT_LOCK    3
#define PROC_STATE_PAUSED       4
#define PROC_STATE_SLEEP        5

#define KERNEL_CS 0x08
#define KERNEL_SS 0x10

#define USER_CS 0x1b
#define USER_SS 0x23

typedef struct proc
{
    uint64_t pid;
    void* addr_space;
    uint8_t state;
    uint64_t sleep_exp;
    struct proc* next;

    reg_ctx_t regs;

} proc_t;

extern proc_t* sleep_tsk_lst;
extern proc_t* last_proc;

// shed.c
void sched_init();
void schedule(reg_ctx_t* r);
proc_t* mk_proc(void* entry);
void sched_tick(reg_ctx_t* r);
void sched_spawn_proc(proc_t* proc);
void sched_kill_proc();
void sched_block(uint32_t state);
void sched_unblock(proc_t* proc);

// exec.c
proc_t* mk_elf_proc(uint8_t* elf_dat);

// sleep.c
void nano_sleep(uint64_t ns);
void sleep(uint64_t s);

/*typedef struct semaphore
{
    uint32_t max_cnt;
    uint32_t curr_cnt;
    proc_t* first_wait_tsk;
    proc_t* last_wait_tsk;

} semaphore_t;

// lock.c
semaphore_t* mk_semaphore(uint32_t max_cnt);
void acquire_semaphore(semaphore_t* sem);
void release_semaphore(semaphore_t* sem);*/

