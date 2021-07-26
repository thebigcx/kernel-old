#include <fs/vfs/vfs.h>
#include <fs/fat/fat.h>
#include <fs/ext2/ext2.h>
#include <mem/kheap.h>
#include <util/stdlib.h>

int vfs_get_type(vfs_node_t* dev)
{
    if (fat_is_fat(dev))
    {
        return FS_TYPE_FAT32;// TODO: FAT12/16
    }
    else if (ext2_is_ext2(dev))
    {
        return FS_TYPE_EXT2;
    }
}

void vfs_mount_recur(vfs_path_t* path, tree_node_t* node, vfs_node_t* dev, uint32_t depth)
{
    if (path->parts->cnt == depth)
    {
        vfs_ent_t* ent = (vfs_ent_t*)node->data;
        ent->file = dev;
        return;
    }

    bool found = false;
    char* name = (char*)list_get(path->parts, depth)->val;

    list_foreach(node->children, child)
    {
        tree_node_t* tnode = (tree_node_t*)child->val;
        vfs_ent_t* ent = (vfs_ent_t*)tnode->data;

        if (strcmp(ent->name, name) == 0)
        {
            found = true;
            node = tnode;
            break;
        }
    }

    if (!found)
    {
        vfs_ent_t* ent = kmalloc(sizeof(vfs_ent_t));
        ent->name = strdup(name);

        node = tree_insert(vfs_tree, node, ent);
    }

    vfs_mount_recur(path, node, dev, ++depth);
}

void vfs_mount(vfs_node_t* dev, const char* mnt_pt)
{
    vfs_path_t* path = vfs_mkpath(mnt_pt, NULL);

    if (path->parts->cnt == 0) // Root directory
    {
        ((vfs_ent_t*)vfs_tree->root->data)->file = dev;

        vfs_destroy_path(path);
        return;
    }
    
    vfs_mount_recur(path, vfs_tree->root, dev, 0);

    vfs_destroy_path(path);
}

vfs_node_t* vfs_get_mountpoint_recur(tree_node_t* node, vfs_path_t* path, uint32_t depth)
{
    if (path->parts->cnt == depth)
    {
        vfs_ent_t* ent = node->data;
        return ent->file;
    }

    bool found = false;
    char* name = (char*)list_get(path->parts, depth)->val;
    
    list_foreach(node->children, child)
    {
        tree_node_t* tnode = (tree_node_t*)child->val;
        vfs_ent_t* ent = (vfs_ent_t*)tnode->data;
        
        if (strcmp(ent->name, name) == 0)
        {
            found = true;
            node = tnode;
            break;
        }
    }

    if (!found)
    {
        return ((vfs_ent_t*)node->data)->file;
    }

    return vfs_get_mountpoint_recur(node, path, ++depth);
}

vfs_node_t* vfs_get_mountpoint(vfs_path_t* path)
{
    if (path->parts->cnt == 0)
    {
        return (vfs_node_t*)((vfs_ent_t*)vfs_tree->root->data)->file;
    }
    
    return vfs_get_mountpoint_recur(vfs_tree->root, path, 0);
}