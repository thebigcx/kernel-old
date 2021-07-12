#pragma once

#include <stdint.h>
#include <stddef.h>

void* memmap(void* addr, size_t len, int prot, int flags, int fd, uint64_t offset);