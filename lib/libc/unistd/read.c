#include <unistd.h>
#include <sys/syscall.h>

size_t read(int fd, void* buf, size_t cnt)
{
    return syscall(SYS_READ, fd, buf, cnt);
}