#include <drivers/fs/vfs/vfs.h>
#include <drivers/fs/fat/fat.h>
#include <drivers/fs/ext2/ext2.h>
#include <mem/heap.h>

#include <util/stdlib.h>

fs_vol_t* root_vol;
mount_lst_t fs_mnts;

int fs_get_type(dev_t* dev)
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

fs_vol_t* fs_mnt_dev(dev_t* dev, const char* mnt_pt)
{
    fs_vol_t* mnt = kmalloc(sizeof(fs_vol_t));
    //mnt->type = fs_get_type(dev);
    //mnt->type = FS_TYPE_EXT2;
    mnt->type = FS_TYPE_FAT32;
    strcpy(mnt->mnt_pt, mnt_pt);

    if (mnt->type == FS_TYPE_FAT32)
    {
        mnt->find_file = fat_find_file;

        mnt->derived = kmalloc(sizeof(fat_vol_t));
        fat_init((fat_vol_t*)mnt->derived, dev);
    }
    else if (mnt->type == FS_TYPE_EXT2)
    {
        mnt->find_file = ext2_find_file;
        
        mnt->derived = kmalloc(sizeof(ext2_vol_t));
        ext2_init((ext2_vol_t*)mnt->derived, dev);
    }

    fs_mnts.mnts[fs_mnts.cnt++] = mnt;
    return mnt;
}