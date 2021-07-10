#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>

uint64_t syscall(uint64_t sysno, ...);
size_t write(int fd, const void* buf, size_t cnt);
size_t read(int fd, void* buf, size_t cnt);
int open(const char* path, int flags, uint32_t mode);
int close(int fd);
void* mmap(void* addr, size_t len, int prot, int flags, int fd, uint64_t offset);
int stat(const char* path, stat_t* stat);
int fork();
int exec(const char* path, int argc, char** argv);
int ioctl(int fd, uint64_t request, void* argp);
int waitpid(int pid);
void exit(int code);
void sleepns(uint64_t ns);
void sleepus(uint64_t us);
void sleepms(uint64_t ms);
void sleeps(uint64_t s);
int seek(int fd, int64_t off, int whence);