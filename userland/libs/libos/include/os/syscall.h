#pragma once

#include <stdint.h>

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

uint64_t syscall(uint64_t sysno, ...);