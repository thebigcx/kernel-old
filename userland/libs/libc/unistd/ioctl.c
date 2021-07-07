#include <unistd.h>
#include <sys/syscall.h>

int ioctl(int fd, uint64_t request, void* argp)
{
    return syscall(SYS_IOCTL, fd, request, argp);
}