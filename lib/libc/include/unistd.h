#pragma once

#include <stdint.h>
#include <stddef.h>

uint64_t syscall(uint64_t sysno, ...);
size_t write(int fd, const void* buf, size_t cnt);
size_t read(int fd, void* buf, size_t cnt);