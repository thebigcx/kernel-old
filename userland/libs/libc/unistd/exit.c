#include <unistd.h>
#include <sys/syscall.h>

void exit(int code)
{
    syscall(SYS_EXIT, code);
}