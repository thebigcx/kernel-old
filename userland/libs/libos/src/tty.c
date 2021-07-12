#include <os/tty.h>
#include <os/syscall.h>

int pty_open(int* master)
{
    return syscall(SYS_OPENPTY, master);
}