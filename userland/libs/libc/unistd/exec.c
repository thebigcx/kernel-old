#include <unistd.h>
#include <sys/syscall.h>

int fork()
{
    return syscall(SYS_FORK);
}

int exec(const char* path, char** args)
{
    return syscall(SYS_EXEC, path, args);
}