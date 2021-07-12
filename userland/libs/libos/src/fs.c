#include <os/fs.h>
#include <os/syscall.h>

size_t file_write(int fd, const void* buf, size_t cnt)
{
    return syscall(SYS_WRITE, fd, buf, cnt);
}

size_t file_read(int fd, void* buf, size_t cnt)
{
    return syscall(SYS_READ, fd, buf, cnt);
}

int file_open(const char* path, int flags, uint32_t mode)
{
    return syscall(SYS_OPEN, path, flags, mode);
}

int file_close(int fd)
{
    return syscall(SYS_CLOSE, fd);
}

int file_stat(const char* path, stat_t* stat)
{
    return syscall(SYS_STAT, path, stat);
}

int file_ioctl(int fd, uint64_t request, void* argp)
{
    return syscall(SYS_IOCTL, fd, request, argp);
}

int file_seek(int fd, int64_t off, int whence)
{
    return syscall(SYS_SEEK, fd, off, whence);
}
