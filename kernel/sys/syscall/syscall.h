#pragma once

#include <sys/system.h>

#define SYS_READ  0
#define SYS_WRITE 1
#define SYS_OPEN  2
#define SYS_CLOSE 3
#define SYS_MMAP  4
#define SYS_IOCTL 5

typedef uint64_t(*syscall_t)(reg_ctx_t*);

void syscall_handler(reg_ctx_t* regs);