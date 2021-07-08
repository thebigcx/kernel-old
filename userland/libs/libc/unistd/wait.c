#include <unistd.h>
#include <sys/syscall.h>

int waitpid(int pid)
{
    return syscall(SYS_WAITPID, pid);
}