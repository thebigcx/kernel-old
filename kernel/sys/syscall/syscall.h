#pragma once

#include <sys/system.h>

#define SYS_READ        0
#define SYS_WRITE       1
#define SYS_OPEN        2
#define SYS_CLOSE       3
#define SYS_MMAP        4
#define SYS_IOCTL       5
#define SYS_STAT        6
#define SYS_FORK        7
#define SYS_EXEC        8
#define SYS_WAITPID     9
#define SYS_EXIT        10
#define SYS_SLEEPNS     11
#define SYS_SEEK        12
#define SYS_OPENPTY     13
#define SYS_THREADCREAT 14
#define SYS_THREADEXIT  15

#define SEEK_SET        1
#define SEEK_CUR        2
#define SEEK_END        3

void syscall_handler(reg_ctx_t* regs);