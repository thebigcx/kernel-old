#pragma once

#include <sys/system.h>

#define THREAD_STATE_RUNNING     0
#define THREAD_STATE_READY       1
#define THREAD_STATE_WAITING     2
#define THREAD_STATE_WAIT_LOCK   3
#define THREAD_STATE_PAUSED      4
#define THREAD_STATE_ASLEEP       5
#define THREAD_STATE_KILLED      6

typedef struct proc proc_t;

typedef struct thread
{
    proc_t* parent;
    int state;
    reg_ctx_t regs;
    uint64_t sleepexp; // If asleep, contains the expiry time point in nanoseconds

} thread_t;

void thread_block(int state);
void thread_unblock(thread_t* thread);
void thread_sleepuntil(uint64_t timepoint);
void thread_sleepns(uint64_t ns);
void thread_kill();
