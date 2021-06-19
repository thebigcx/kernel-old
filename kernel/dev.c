#include <dev.h>
#include <drivers/fs/vfs/vfs.h>
#include <util/rand.h>

int devopen(fs_node_t* file) { return 0; }
void devclose(fs_node_t* file) {}

size_t urandom_read(fs_node_t* node, void* ptr, size_t off, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        uint8_t n = rand_range(0, 255);
        *((uint8_t*)ptr + i) = n;
    }
    
    return 1;
}

size_t urandom_write(fs_node_t* node, const void* ptr, size_t off, size_t size) { return 0; }

size_t null_read(fs_node_t* node, void* ptr, size_t off, size_t size)
{
    for (size_t i = 0; i < size; i++)
        *((uint8_t*)ptr + i) = 0;
    
    return 1;
}

size_t null_write(fs_node_t* node, const void* ptr, size_t off, size_t size) { return 0; }

void dev_init()
{
    fs_node_t urandom;
    urandom.read = urandom_read;
    urandom.write = urandom_write;
    urandom.open = devopen;
    urandom.close = devclose;

    vfs_mk_dev_file(urandom, "/dev/urandom");

    fs_node_t null;
    null.read = null_read;
    null.write = null_write;
    null.open = devopen;
    null.close = devclose;

    vfs_mk_dev_file(null, "/dev/null");
}