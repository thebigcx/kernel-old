#include <unistd.h>
#include <sys/syscall.h>

int openpty(int* master)
{
    return syscall(SYS_OPENPTY, master);
}