#include <drivers/fs/vfs/vfs.h>
#include <drivers/fs/fat/fat.h>
#include <mem/heap.h>
#include <stdlib.h>
#include <string.h>

mount_t* root_mnt_pt;
mount_lst_t fs_mnts;

int fs_get_type(dev_t* dev)
{
    return FS_TYPE_FAT32; // TODO: other filesystems
}

mount_t* fs_mnt_dev(dev_t* dev, const char* mnt_pt)
{
    mount_t* mnt = kmalloc(sizeof(mount_t));
    mnt->fs_dri.type = FS_TYPE_FAT32;
    mnt->dev = dev;
    strcpy(mnt->mnt_pt, mnt_pt);

    if (fs_get_type(dev) == FS_TYPE_FAT32)
    {
        mnt->fs_dri.find_file = fat_find_file;

        mnt->fs_dri.priv = kmalloc(sizeof(fat_vol_t));
        fat_init((fat_vol_t*)mnt->fs_dri.priv, dev);
    }

    fs_mnts.mnts[fs_mnts.cnt++] = mnt;
    return mnt;
}