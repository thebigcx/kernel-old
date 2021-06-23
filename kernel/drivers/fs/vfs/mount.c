#include <drivers/fs/vfs/vfs.h>
#include <drivers/fs/fat/fat.h>
#include <drivers/fs/ext2/ext2.h>
#include <mem/heap.h>
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

void vfs_mount(vfs_node_t* dev, const char* mnt_pt)
{
    vfs_path_t* path = vfs_mkpath(mnt_pt, NULL);

    if (path->parts->cnt == 0) // Root directory
    {
        vfs_root = kmalloc(sizeof(vfs_node_t));
        vfs_root->device = kmalloc(sizeof(ext2_vol_t));
        ext2_init(vfs_root->device, dev);
        vfs_root->finddir = ext2_finddir;
        vfs_root->inode_num = 2;
    }

    vfs_destroy_path(path);

    /*fs_vol_t* mnt = kmalloc(sizeof(fs_vol_t));
    //mnt->type = fs_get_type(dev);
    mnt->type = FS_TYPE_EXT2;
    //mnt->type = FS_TYPE_FAT32;
    strcpy(mnt->mnt_pt, mnt_pt);

    if (mnt->type == FS_TYPE_FAT32)
    {
        mnt->finddir = fat_find_file;

        mnt->derived = kmalloc(sizeof(fat_vol_t));
        fat_init((fat_vol_t*)mnt->derived, dev);
    }
    else if (mnt->type == FS_TYPE_EXT2)
    {
        mnt->finddir = ext2_finddir;
        
        mnt->derived = kmalloc(sizeof(ext2_vol_t));
        ext2_init((ext2_vol_t*)mnt->derived, dev);
    }

    fs_mnts.mnts[fs_mnts.cnt++] = mnt;
    return mnt;*/
}

vfs_node_t* vfs_get_mountpoint(vfs_path_t* path)
{
    //if (path->parts->cnt == 0)
    {
        return vfs_root;
    }
}