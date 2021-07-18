#include <unistd.h>
#include <os/mem.h>

void* mmap(void* addr, size_t length, int prot, int flags, int fd, off_t offset)
{
    return memmap(addr, length, prot, flags, fd, offset);
}

int munmap(void* addr, size_t length)
{

}