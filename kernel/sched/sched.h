#pragma once

#include <util/types.h>
#include <sys/system.h>
#include <util/list.h>
#include <util/tree.h>
#include <mem/paging.h>

#define PROC_STATE_RUNNING      0
#define PROC_STATE_READY        1
#define PROC_STATE_WAITING      2
#define PROC_STATE_WAIT_LOCK    3
#define PROC_STATE_PAUSED       4
#define PROC_STATE_SLEEP        5
#define PROC_STATE_KILLED       6

#define KERNEL_CS 0x08
#define KERNEL_SS 0x10

#define USER_CS 0x1b
#define USER_SS 0x23

typedef struct proc
{
    uint64_t pid; // Process ID
    page_map_t* addr_space; // Virtual address space
    uint8_t state;
    uint64_t sleep_exp; // Sleep expiry (if state = PROC_STATE_SLEEP)
    list_t* file_descs; // Open file descriptors

    char* name; // Name
    char* working_dir; // Working directory

    reg_ctx_t regs; // Register context

    tree_node_t* treenode; // Node in the tree this process belongs to

} proc_t;

// shed.c
void sched_init();
void schedule(reg_ctx_t* r);
proc_t* mk_proc(void* entry);
void sched_tick(reg_ctx_t* r);
void sched_spawn(proc_t* proc, proc_t* parent);
void sched_terminate();
void sched_proc_destroy(proc_t* proc);
void sched_block(uint32_t state);
void sched_unblock(proc_t* proc);
void sched_fork(proc_t* proc, reg_ctx_t* regs);
void sched_exec(const char* path, int argc, char** argv);
list_t* sched_getprocs();

proc_t* sched_get_currproc();

proc_t* mkelfproc(const char* path, int argc, char** argv, int envp, char** env);

void sched_sleepns(uint64_t ns);
void sleep(uint64_t s);

typedef struct sem
{
    uint32_t max_cnt;
    uint32_t curr_cnt;
    list_t* waitlst;

} sem_t;

sem_t* sem_create(uint32_t max_cnt);
void sem_acquire(sem_t* sem);
void sem_release(sem_t* sem);

// A semaphore of max_cnt = 1
// Faster than a semaphore and less memory (pretty much neglible though)
typedef struct mutex
{
    uint8_t proc; // bool: true if a process has acquired the mutex
    list_t* waitlst;

} mutex_t;

mutex_t* mutex_create();
void mutex_acquire(mutex_t* mutex);
void mutex_release(mutex_t* mutex);