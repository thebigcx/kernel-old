#pragma once

#include <sys/system.h>

#define SYS_OPEN  0
#define SYS_READ  1
#define SYS_WRITE 2
#define SYS_CLOSE 3

typedef uint64_t(*syscall_t)(reg_ctx_t*);

void syscall_handler(reg_ctx_t* regs);