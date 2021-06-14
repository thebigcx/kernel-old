#include <drivers/fs/ext2/ext2.h>

void ext2_write_inode(ext2_vol_t* vol, ext2_inode_t* inode, uint32_t idx, dev_t* dev)
{
    uint32_t group = idx / vol->super.inode_cnt;
}

void ext2_read_inode(ext2_vol_t* vol, uint32_t num, ext2_inode_t* inode)
{
    uint8_t* buf = kmalloc(vol->blk_sz);
    uint64_t lba = ext2_inode_lba(vol, num);

    vol->dev->read(vol->dev, lba, 1, buf);

    memcpy(inode, (buf + (ext2_inode_bg_idx(vol, num) * vol->superext.inode_sz) % 512), sizeof(ext2_inode_t));
    kfree(buf);
}

uint32_t* ext2_get_inode_blks(ext2_vol_t* vol, uint32_t idx, uint32_t cnt, ext2_inode_t* ino)
{
    uint32_t* ret = kmalloc(cnt * sizeof(uint32_t));

    // TODO: double and triple indirect blocks
    for (uint32_t i = 0; i < cnt; i++)
    {
        ret[i] = ino->blocks[i];
    }

    return ret;
}