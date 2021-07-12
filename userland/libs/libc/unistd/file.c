#include <unistd.h>
#include <os/fs.h>

ssize_t read(int fd, void* buf, size_t count)
{
    return file_read(fd, buf, count);
}

ssize_t write(int fd, const void* buf, size_t count)
{
    return file_write(fd, buf, count);
}

int open(const char* pathname, int flags, mode_t mode)
{
    return file_open(pathname, flags, mode);
}

int close(int fd)
{
    return file_close(fd);
}

off_t lseek(int fd, off_t offset, int whence)
{
    return file_seek(fd, offset, whence);
}