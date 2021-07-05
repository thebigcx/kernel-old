#include <unistd.h>
#include <sys/syscall.h>

int open(const char* path, int flags, unsigned int mode)
{
    return syscall(SYS_OPEN, flags, mode);
}