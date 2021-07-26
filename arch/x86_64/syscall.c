#include <arch/x86_64/syscall.h>

void arch_syscall_ret(reg_ctx_t* r, uint64_t val)
{
	r->rax = val;
}

uint64_t arch_syscall_num(reg_ctx_t* r)
{
	return r->rax;
}
