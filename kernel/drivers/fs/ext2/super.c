#include <drivers/fs/ext2/ext2.h>

void ext2_get_root(ext2_vol_t* vol, vfs_node_t* root, ext2_inode_t* ino)
{
    root->device = vol;

    root->open = ext2_open;
    root->close = ext2_close;
    root->read = ext2_read;
    root->write = ext2_write;
    root->finddir = ext2_finddir;
    root->listdir = ext2_listdir;
    root->mkfile = ext2_mkfile;
    root->mkdir = ext2_mkdir;

    root->flags = FS_DIR;
    root->inode_num = 2;
    root->size = ext2_get_size(ino);
    root->name = strdup("/");
}

vfs_node_t* ext2_init(vfs_node_t* dev)
{
    vfs_node_t* node = kmalloc(sizeof(vfs_node_t));
    ext2_vol_t* vol = kmalloc(sizeof(ext2_vol_t));

    //vfs_node_t* dev = vfs_resolve_path(path, NULL);

    dev->read(dev, &vol->super, EXT2_SB_LBA, 1);
    vol->dev = dev;
    vol->blk_sz = 1024u << vol->super.log_block_sz;

    vol->blk_grp_cnt = (vol->super.blk_cnt % vol->super.blks_per_grp) ? (vol->super.blk_cnt / vol->super.blks_per_grp + 1) : (vol->super.blk_cnt / vol->super.blks_per_grp);
    vol->blk_grps = kmalloc(vol->blk_grp_cnt * sizeof(ext2_bgd_t) + 512);

    uint64_t block_glba = ext2_blk_to_lba(vol, ext2_loc_to_blk(vol, EXT2_SB_LOC) + 1);
    uint32_t sectors = vol->blk_grp_cnt * sizeof(ext2_bgd_t) / 512 + 1;
    dev->read(dev, vol->blk_grps, block_glba, sectors);
    
    ext2_inode_t ino;
    ext2_read_inode(vol, 2, &ino);
    ext2_get_root(vol, node, &ino);

    return node;
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
    return (inode - 1) % vol->super.inodes_per_grp;
}

uint32_t ext2_inode_bg(ext2_vol_t* vol, uint32_t inode)
{
    return (inode - 1) / vol->super.inodes_per_grp;
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

uint32_t ext2_alloc_block(ext2_vol_t* vol)
{
    uint8_t* buf = kmalloc(vol->blk_sz);

    size_t blocks = vol->blk_grp_cnt;
    for (uint32_t i = 0; i < blocks; i++)
    {
        uint32_t grp = vol->blk_grps[i].block_bitmap_addr;
        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, grp), vol->blk_sz / 512);

        for (uint32_t j = 0; j < vol->blk_sz; j++)
        {
            if (buf[j] == 0xff) continue;

            for (uint32_t k = 0; k < 8; k++)
            {
                if ((buf[j] & (1 << k)) == 0)
                {
                    buf[j] |= (1 << k);
                    vol->dev->write(vol->dev, buf, ext2_blk_to_lba(vol, grp), vol->blk_sz / 512);

                    vol->blk_grps[i].free_blocks--;
                    ext2_rewrite_bgds(vol);

                    kfree(buf);
                    return i * vol->super.blks_per_grp + j * 8 + k;
                }
            }
        }
    }

    kfree(buf);
    return 0;
}

void ext2_free_block(ext2_vol_t* vol, uint32_t blk)
{

}

// TODO: support more than one block of group descriptors
void ext2_rewrite_bgds(ext2_vol_t* vol)
{
    uint32_t bgds_loc = 2 * (vol->blk_sz / 512);
    //for (uint32_t i = 0; i < vol->blk_grp_cnt; i++)
    {
        uint8_t* buffer = kmalloc(vol->blk_sz);

        vol->dev->read(vol->dev, buffer, bgds_loc, vol->blk_sz / 512);
        memcpy(buffer, vol->blk_grps, vol->blk_grp_cnt * sizeof(ext2_bgd_t));
        vol->dev->write(vol->dev, buffer, bgds_loc, vol->blk_sz / 512);
    }
}