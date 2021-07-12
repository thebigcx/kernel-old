#include <os/mem.h>
#include <os/syscall.h>

void* memmap(void* addr, size_t len, int prot, int flags, int fd, uint64_t offset)
{
    return syscall(SYS_MMAP, addr, len, prot, flags, fd, offset);
}