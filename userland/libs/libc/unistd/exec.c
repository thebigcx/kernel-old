#include <unistd.h>
#include <sys/syscall.h>

int fork()
{
    return syscall(SYS_FORK);
}

int exec(const char* path, int argc, char** args)
{
    return syscall(SYS_EXEC, path, argc, args);
}