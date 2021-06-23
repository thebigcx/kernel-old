#include <drivers/fs/vfs/vfs.h>
#include <util/stdlib.h>
#include <mem/heap.h>

tree_t* vfs_tree;
vfs_node_t* vfs_root;

void vfs_init()
{
    vfs_tree = tree_create();
    vfs_ent_t* root = kmalloc(sizeof(vfs_ent_t));
    root->name = strdup("root");
    root->file = NULL;
    tree_insert(vfs_tree, vfs_tree->root, root);
}

// Special function for traversing mounted
// filesystems as well as physical fs.
vfs_node_t open_file(char* name, uint32_t flags)
{
    
}

fs_fd_t* vfs_open(vfs_node_t* file, uint32_t flags)
{
    return file->open(file, flags);
}

void vfs_close(vfs_node_t* file)
{
    file->close(file);
}

size_t vfs_read(vfs_node_t* file, void* ptr, size_t off, size_t size)
{
    return file->read(file, ptr, off, size);
}

size_t vfs_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{
    return file->write(file, ptr, off, size);
}

vfs_node_t vfs_resolve_path(const char* pathstr, const char* working_dir)
{
    vfs_path_t* path = vfs_mkpath(pathstr, working_dir);

    vfs_node_t node = *vfs_get_mountpoint(path);

    list_foreach(path->parts, part)
    {
        node = node.finddir(&node, part->val);
    }

    vfs_destroy_path(path);

    return node;
}