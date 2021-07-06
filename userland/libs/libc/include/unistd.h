#pragma once

#include <stdint.h>
#include <stddef.h>
#include <sys/stat.h>

uint64_t syscall(uint64_t sysno, ...);
size_t write(int fd, const void* buf, size_t cnt);
size_t read(int fd, void* buf, size_t cnt);
int open(const char* path, int flags, unsigned int mode);
int close(int fd);
void* mmap(void* addr, size_t len, int prot, int flags, int fd, uint64_t offset);
int stat(const char* path, stat_t* stat);
int fork();
int exec(const char* path, char** args);