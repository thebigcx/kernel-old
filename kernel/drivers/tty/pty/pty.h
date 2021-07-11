#pragma once

#include <drivers/fs/vfs/vfs.h>

enum PTYTYPE
{
    PTYTYPE_MASTER,
    PTYTYPE_SLAVE
};

typedef struct pty
{
    vfs_node_t* master_file;
    vfs_node_t* slave_file;

} pty_t;

pty_t* pty_grant();

size_t pty_master_read(vfs_node_t* file, void* ptr, size_t off, size_t size);
size_t pty_slave_read(vfs_node_t* file, void* ptr, size_t off, size_t size);

size_t pty_master_write(vfs_node_t* file, const void* ptr, size_t off, size_t size);
size_t pty_slave_write(vfs_node_t* file, const void* ptr, size_t off, size_t size);

size_t pty_read(vfs_node_t* file, void* ptr, size_t off, size_t size);
size_t pty_write(vfs_node_t* file, const void* ptr, size_t off, size_t size);