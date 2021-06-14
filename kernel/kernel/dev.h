#pragma once

#include <types.h>
#include <drivers/storage/ahci.h>

// Derived from fs_node_t
typedef struct dev
{
    int (*read)(struct dev* dev, uint64_t offset, uint32_t len, void* buffer);
    int (*write)(struct dev* dev, uint64_t offset, uint32_t len, void* buffer);
    void* derived;

} dev_t;

// Derived from dev_t
typedef struct dskdev
{
    ahci_port_t* port;

} dskdev_t;

// Initialize device files
void dev_init();