#include <drivers/fs/vfs/vfs.h>
#include <drivers/fs/fat/fat.h>
#include <stdlib.h>

mount_t root_mnt_pt;

int fs_get_type(storage_dev_t* dev)
{
    return FS_TYPE_FAT32; // TODO: other filesystems
}

void fs_mnt_disk(storage_dev_t* dev, mount_t* mnt)
{
    int type = fs_get_type(dev);
    mnt->fs_dri.type = type;
    mnt->dev = dev;

    if (type == FS_TYPE_FAT32)
    {
        mnt->fs_dri.fopen = fat_fopen;
        mnt->fs_dri.fread = fat_fread;
        mnt->fs_dri.fwrite = fat_fwrite;
        mnt->fs_dri.fclose = fat_fclose;

        mnt->fs_dri.priv = malloc(sizeof(fat_dri_t));
        fat_init((fat_dri_t*)mnt->fs_dri.priv, dev);
    }
}