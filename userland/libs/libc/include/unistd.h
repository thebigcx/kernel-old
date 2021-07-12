#pragma once

#include <sys/types.h>

ssize_t read(int fd, void* buf, size_t count);
ssize_t write(int fd, const void* buf, size_t count);
int open(const char* pathname, int flags, mode_t mode);
int close(int fd);
off_t lseek(int fd, off_t offset, int whence);
void exit(int status);
int execve(const char* pathname, char* const argv[], char* const envp[]);
//int stat(const char* restrict pathname, struct stat* restrict statbuf);