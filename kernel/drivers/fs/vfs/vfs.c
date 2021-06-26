#include <drivers/fs/vfs/vfs.h>
#include <util/stdlib.h>
#include <mem/heap.h>

tree_t* vfs_tree;

void vfs_init()
{
    vfs_tree = tree_create();
    vfs_ent_t* root = kmalloc(sizeof(vfs_ent_t));
    root->name = strdup("root");
    root->file = NULL;
    vfs_tree->root->data = root;
}

fs_fd_t* vfs_open(vfs_node_t* file, uint32_t flags)
{
    if (file->open)
        file->open(file, flags);

    fs_fd_t* fd = kmalloc(sizeof(fs_fd_t));
    fd->node = file;
    fd->pos = 0;
    fd->flags = flags;
    return fd;
}

void vfs_close(vfs_node_t* file)
{
    if (file->close)
        file->close(file);
}

size_t vfs_read(vfs_node_t* file, void* ptr, size_t off, size_t size)
{
    if (file->read)
        return file->read(file, ptr, off, size);

    return 0;
}

size_t vfs_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{
    if (file->write)
        return file->write(file, ptr, off, size);

    return 0;
}

vfs_node_t* vfs_resolve_path(const char* pathstr, const char* working_dir)
{
    vfs_path_t* path = vfs_mkpath(pathstr, working_dir);
    vfs_node_t* node = vfs_get_mountpoint(path);

    if (node->flags == FS_BLKDEV || node->flags == FS_CHARDEV) // Device file
    {
        return node;
    }

    list_foreach(path->parts, part)
    {
        vfs_node_t* next = node->finddir(node, part->val);
        kfree(node);
        node = next;
    }

    vfs_destroy_path(path);

    return node;
}