#include <drivers/fs/vfs/vfs.h>
#include <drivers/fs/fat/fat.h>
#include <mem/heap.h>
#include <stdlib.h>
#include <string.h>

fs_vol_t* root_vol;
mount_lst_t fs_mnts;

int fs_get_type(dev_t* dev)
{
    return FS_TYPE_FAT32; // TODO: other filesystems
}

fs_vol_t* fs_mnt_dev(dev_t* dev, const char* mnt_pt)
{
    fs_vol_t* mnt = kmalloc(sizeof(fs_vol_t));
    mnt->type = FS_TYPE_FAT32;
    strcpy(mnt->mnt_pt, mnt_pt);

    if (fs_get_type(dev) == FS_TYPE_FAT32)
    {
        mnt->find_file = fat_find_file;

        mnt->derived = kmalloc(sizeof(fat_vol_t));
        fat_init((fat_vol_t*)mnt->derived, dev);
    }

    fs_mnts.mnts[fs_mnts.cnt++] = mnt;
    return mnt;
}