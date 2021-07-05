#pragma once

#include <stdint.h>

typedef struct stat
{
    uint64_t ino;
    uint32_t mode;
    uint64_t size;

} stat_t;