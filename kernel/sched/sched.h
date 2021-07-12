#pragma once

#include <util/types.h>
#include <sys/system.h>
#include <util/list.h>
#include <util/tree.h>
#include <mem/paging.h>

#define KERNEL_CS 0x08
#define KERNEL_SS 0x10

#define USER_CS 0x1b
#define USER_SS 0x23

typedef struct proc
{
    uint64_t pid; // Process ID
    page_map_t* addr_space; // Virtual address space
    list_t* file_descs; // Open file descriptors

    char* name; // Name
    char* working_dir; // Working directory

    list_t* threads;
    int nexttid;

    list_t* children; // Child processes (created with fork())
    struct proc* parent; // Parent

} proc_t;

void schedule(reg_ctx_t* r);

void sched_tick(reg_ctx_t* r);
void sched_spawn(proc_t* proc, proc_t* parent);
void sched_terminate();
void sched_proc_destroy(proc_t* proc);

void sched_fork(proc_t* proc, reg_ctx_t* regs);
void sched_exec(const char* path, int argc, char** argv);

proc_t* sched_get_currproc();

proc_t* sched_mkproc(void* entry);
proc_t* sched_mkelfproc(const char* path, int argc, char** argv, int envp, char** env);

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