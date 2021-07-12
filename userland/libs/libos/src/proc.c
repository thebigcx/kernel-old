#include <os/proc.h>
#include <os/syscall.h>

int proc_fork()
{
    return syscall(SYS_FORK);
}

int proc_exec(const char* path, int argc, char** args)
{
    return syscall(SYS_EXEC, path, argc, args);
}

int proc_waitpid(int pid)
{
    return syscall(SYS_WAITPID, pid);
}

void proc_exit(int code)
{
    syscall(SYS_EXIT, code);
}