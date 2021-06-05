#include <drivers/fs/ext2/ext2.h>

void ext2_init(ext2_vol_t* vol, dev_t* dev)
{
    dev->read(dev, EXT2_SB_LBA, 2, &vol->super);
}