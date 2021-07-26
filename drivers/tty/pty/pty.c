#include <drivers/tty/pty/pty.h>
#include <mem/kheap.h>

size_t pty_read(vfs_node_t* file, void* ptr, size_t off, size_t size)
{
    pty_t* pty = (pty_t*)file->device;

    if (file == pty->master_file)
    {
        return pty_slave_read(pty->slave_file, ptr, off, size);
    }
    else
    {
        return pty_master_read(pty->master_file, ptr, off, size);
    }
}

size_t pty_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{
    pty_t* pty = (pty_t*)file->device;

    if (file == pty->master_file)
    {
        return pty_slave_write(pty->slave_file, ptr, off, size);
    }
    else
    {
        return pty_master_write(pty->master_file, ptr, off, size);
    }
}

size_t pty_master_read(vfs_node_t* file, void* ptr, size_t off, size_t size)
{
    pty_t* pty = (pty_t*)file->device;
    //return pty->master->read(pty->master, ptr, off, size);
}

size_t pty_slave_read(vfs_node_t* file, void* ptr, size_t off, size_t size)
{

}

size_t pty_master_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{

}

size_t pty_slave_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{

}

pty_t* pty_grant()
{
    pty_t* pty = kmalloc(sizeof(pty_t));
    
    vfs_node_t* master = kmalloc(sizeof(vfs_node_t));
    master->read = pty_read;
    master->write = pty_write;
    master->device = pty;

    vfs_node_t* slave = kmalloc(sizeof(vfs_node_t));
    slave->read = pty_read;
    slave->write = pty_write;
    slave->device = pty;

    pty->master_file = master;
    pty->slave_file = slave;

    return pty;
}