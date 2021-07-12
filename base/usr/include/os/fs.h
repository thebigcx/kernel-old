#pragma once

#include <stdint.h>
#include <stddef.h>

#define STDIN_FD    0
#define STDOUT_FD   1
#define STDERR_FD   2

typedef struct stat
{
    uint64_t ino;
    uint32_t mode;
    uint64_t size;

} stat_t;

size_t file_write(int fd, const void* buf, size_t cnt);
size_t file_read(int fd, void* buf, size_t cnt);
int file_open(const char* path, int flags, uint32_t mode);
int file_close(int fd);
int file_stat(const char* path, stat_t* stat);
int file_ioctl(int fd, uint64_t request, void* argp);
int file_seek(int fd, int64_t off, int whence);