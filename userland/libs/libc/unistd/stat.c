#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>

int stat(const char* path, stat_t* stat)
{
    return syscall(SYS_STAT, path, stat);
}