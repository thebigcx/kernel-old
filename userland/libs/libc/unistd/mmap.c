#include <unistd.h>
#include <sys/syscall.h>

void* mmap(void* addr, size_t len, int prot, int flags, int fd, uint64_t offset)
{
    return syscall(SYS_MMAP, addr, len, prot, flags, fd, offset);
}