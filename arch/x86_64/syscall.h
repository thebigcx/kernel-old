#pragma once

#include <arch/x86_64/system.h>

#define ARCH_SCARG0(r) r->rdi
#define ARCH_SCARG1(r) r->rsi
#define ARCH_SCARG2(r) r->rdx
#define ARCH_SCARG3(r) r->rcx
#define ARCH_SCARG4(r) r->rbx

void arch_syscall_ret(reg_ctx_t* r, uint64_t val);
uint64_t arch_syscall_num(reg_ctx_t* r);

