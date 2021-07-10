#include <unistd.h>
#include <sys/syscall.h>
#include <stdint.h>

int seek(int fd, int64_t off, int whence)
{
    return syscall(SYS_SEEK, fd, off, whence);
}