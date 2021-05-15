#pragma once

#include <stdint.h>

typedef struct storage_dev
{
    int (*read)(struct storage_dev* dev, uint64_t offset, uint32_t len, void* buffer);
    int (*write)(struct storage_dev* dev, uint64_t offset, uint32_t len, void* buffer);
    void* priv;

} storage_dev_t;