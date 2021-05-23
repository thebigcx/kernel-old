#pragma once

#include <stdint.h>
#include <drivers/storage/ahci.h>

typedef struct dev
{
    int (*read)(struct dev* dev, uint64_t offset, uint32_t len, void* buffer);
    int (*write)(struct dev* dev, uint64_t offset, uint32_t len, void* buffer);
    void* derived;

} dev_t;

// Derived class data of dev_t
typedef struct disk_dev
{
    ahci_port_t* port;

} disk_dev_t;