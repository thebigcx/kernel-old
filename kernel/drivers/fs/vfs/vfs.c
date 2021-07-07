#include <drivers/fs/vfs/vfs.h>
#include <util/stdlib.h>
#include <mem/kheap.h>

tree_t* vfs_tree;

void vfs_init()
{
    vfs_tree = tree_create();
    vfs_ent_t* root = kmalloc(sizeof(vfs_ent_t));
    root->name = strdup("root");
    root->file = NULL;
    vfs_tree->root->data = root;
}

fs_fd_t* vfs_open(vfs_node_t* file, uint32_t flags, uint32_t mode)
{
    if (file->open)
        return file->open(file, flags, mode);

    fs_fd_t* fd = kmalloc(sizeof(fs_fd_t));
    fd->node = file;
    fd->pos = 0;
    fd->flags = flags;
    fd->mode = mode;
    return fd;
}

void vfs_close(fs_fd_t* file)
{
    if (file->node->close)
        file->node->close(file->node);
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
    vfs_node_t* mount = vfs_get_mountpoint(path);

    if (mount->flags == FS_BLKDEV || mount->flags == FS_CHARDEV) // Device file
    {
        return mount;
    }

    vfs_node_t* node = mount;

    list_foreach(path->parts, part)
    {
        vfs_node_t* next = node->finddir(node, part->val);
        if (next == NULL) // File does not exist
            return NULL;

        if (node != mount) // Mount point is not kmalloc'd
            kfree(node);

        node = next;
    }

    vfs_destroy_path(path);

    return node;
}

list_t* vfs_listdir(vfs_node_t* dir)
{
    if (dir->listdir)
        return dir->listdir(dir);

    return NULL;
}

void vfs_mkfile(vfs_node_t* parent, const char* name)
{
    if (parent->mkfile)
        parent->mkfile(parent, name);
}

void vfs_mkdir(vfs_node_t* parent, const char* name)
{
    if (parent->mkdir)
        parent->mkdir(parent, name);
}

int vfs_ioctl(vfs_node_t* file, uint64_t request, void* argp)
{
    if (file->ioctl)
        return file->ioctl(file, request, argp);

    return 0;
}

int vfs_stat(const char* path, vfs_stat_t* stat)
{
    proc_t* proc = sched_get_currproc();
    vfs_node_t* node = vfs_resolve_path(path, proc->working_dir);

    if (!node)
    {
        return -1;
    }

    return 0;
}