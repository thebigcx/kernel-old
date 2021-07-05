#include <unistd.h>

void stat(const char* path, stat_t* stat)
{
    return syscall(SYS_STAT, path, stat);
}