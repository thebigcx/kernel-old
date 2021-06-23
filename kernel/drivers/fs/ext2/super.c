#include <drivers/fs/ext2/ext2.h>

void ext2_init(ext2_vol_t* vol, vfs_node_t* dev)
{
    dev->read(dev, &vol->super, EXT2_SB_LBA, 1);
    vol->dev = dev;
    vol->blk_sz = 1024u << vol->super.log_block_sz;

    vol->blk_grp_cnt = (vol->super.blk_cnt % vol->super.blks_per_grp) ? (vol->super.blk_cnt / vol->super.blks_per_grp + 1) : (vol->super.blk_cnt / vol->super.blks_per_grp);
    vol->blk_grps = kmalloc(vol->blk_grp_cnt * sizeof(ext2_group_desc_tbl_t));

    uint64_t block_glba = ext2_blk_to_lba(vol, ext2_loc_to_blk(vol, EXT2_SB_LOC) + 1);
    dev->read(dev, vol->blk_grps, block_glba, vol->blk_grp_cnt * sizeof(ext2_group_desc_tbl_t));
}

uint8_t ext2_is_ext2(vfs_node_t* dev)
{
    ext2_sb_t* sb = kmalloc(512);
    
    dev->read(dev, sb, EXT2_SB_LBA, 1);
    uint16_t sig = sb->sb.ext2_sig;

    kfree(sb);

    return sig == 0xef53;
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