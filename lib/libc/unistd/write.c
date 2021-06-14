#include <unistd.h>
#include <sys/syscall.h>

size_t write(int fd, const void* buf, size_t cnt)
{
    return syscall(SYS_WRITE, fd, buf, cnt);
}