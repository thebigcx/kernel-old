#include <drivers/fs/ext2/ext2.h>

void ext2_init(ext2_vol_t* vol, dev_t* dev)
{
    dev->read(dev, EXT2_SB_LBA, 1, &vol->super);
    vol->dev = dev;
    vol->blk_sz = 1024u << vol->super.log_block_sz;

    vol->blk_grp_cnt = (vol->super.blk_cnt % vol->super.blks_per_grp) ? (vol->super.blk_cnt / vol->super.blks_per_grp + 1) : (vol->super.blk_cnt / vol->super.blks_per_grp);
    vol->blk_grps = kmalloc(vol->blk_grp_cnt * sizeof(ext2_group_desc_tbl_t));

    uint64_t block_glba = ext2_blk_to_lba(vol, ext2_loc_to_blk(vol, EXT2_SB_LOC) + 1);
    dev->read(dev, block_glba,vol->blk_grp_cnt * sizeof(ext2_group_desc_tbl_t), vol->blk_grps);

    ext2_inode_t root;
    ext2_read_inode(vol, vol->superext.first_inode, &root);

    ext2_node_t node = { .ino = root, .vol = vol };

    char buffer[100];
    //dev->read(dev, 104, 1, buffer);
    ext2_read(&node, buffer, 0, 100);

    for (int i = 0; i < 100; i++)
    {
        console_putchar(buffer[i], 255, 255, 255);
    }

    while (1);
}

uint32_t ext2_inode_bg_idx(ext2_vol_t* vol, uint32_t inode)
{
    return (inode - 1) % vol->super.inode_cnt;
}

uint32_t ext2_inode_bg(ext2_vol_t* vol, uint32_t inode)
{
    return (inode - 1) / vol->super.inode_cnt;
}

uint64_t ext2_inode_lba(ext2_vol_t* vol, uint32_t inode)
{
    uint32_t blk = vol->blk_grps[ext2_inode_bg(vol, inode)].inode_tbl_addr;
    uint64_t lba = (blk * vol->blk_sz + ext2_inode_bg_idx(vol, inode) * vol->superext.inode_sz) / 512;

    return lba;
}

uint32_t ext2_loc_to_blk(ext2_vol_t* vol, uint64_t loc)
{
    return (loc >> vol->super.log_block_sz) >> 10;
}

uint64_t ext2_blk_to_lba(ext2_vol_t* vol, uint64_t blk)
{
    return blk * (vol->blk_sz / 512);
}