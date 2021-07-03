#include <drivers/fs/ext2/ext2.h>
#include <util/stdlib.h>
#include <mem/kheap.h>

#define EXT2_INO_SIND 12 // Singly indirect
#define EXT2_INO_DIND 13 // Doubly indirect
#define EXT2_INO_TIND 14 // Triply indirect

void ext2_write_inode(ext2_vol_t* vol, uint32_t num, ext2_inode_t* inode)
{
    uint32_t group = num / vol->super.inodes_per_grp; // Block group
    uint32_t table = vol->blk_grps[group].inode_tbl_addr; // Inode table
    uint32_t offset = (num - 1) * vol->superext.inode_sz / vol->blk_sz; // Offset in table of inode
    
    uint8_t* buf = kmalloc(vol->blk_sz);
    vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, table + offset), vol->blk_sz / 512);

    uint32_t ino_offset = (num - 1) - offset * (vol->blk_sz / vol->superext.inode_sz);
    memcpy(buf + ino_offset * vol->superext.inode_sz, inode, vol->superext.inode_sz);

    vol->dev->write(vol->dev, buf, ext2_blk_to_lba(vol, table + offset), vol->blk_sz / 512);

    kfree(buf);
}

void ext2_read_inode(ext2_vol_t* vol, uint32_t num, ext2_inode_t* inode)
{
    uint8_t* buf = kmalloc(512);
    uint64_t lba = ext2_inode_lba(vol, num);

    vol->dev->read(vol->dev, buf, lba, 1);

    memcpy(inode, (buf + (ext2_inode_bg_idx(vol, num) * vol->superext.inode_sz) % 512), sizeof(ext2_inode_t));
    kfree(buf);
}

uint32_t ext2_get_inode_blk(ext2_vol_t* vol, uint32_t idx, ext2_inode_t* ino)
{
    uint32_t bpp = vol->blk_sz / sizeof(uint32_t);  // Blocks per pointer
    uint32_t sind_strt = EXT2_INO_SIND;             // Singly indirect start
    uint32_t dind_strt = sind_strt + bpp;           // Doubly indirect start
    uint32_t tind_strt = dind_strt + bpp * bpp;     // Triply indirect start

    if (idx < sind_strt)
    {
        return ino->blocks[idx];
    }
    else if (idx < dind_strt)
    {
        uint32_t* buf = kmalloc(vol->blk_sz);

        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, ino->blocks[EXT2_INO_SIND]), vol->blk_sz / 512);

        uint32_t ret = buf[idx - EXT2_INO_SIND];
        kfree(buf);
        return ret;
    }
    else if (idx < tind_strt)
    {
        uint32_t* blk_ptrs = kmalloc(vol->blk_sz);
        uint32_t* buf = kmalloc(vol->blk_sz);

        vol->dev->read(vol->dev, blk_ptrs, ext2_blk_to_lba(vol, ino->blocks[EXT2_INO_DIND]), vol->blk_sz / 512);

        uint32_t blk_ptr = blk_ptrs[(idx - dind_strt) / bpp];
        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, blk_ptr), vol->blk_sz / 512);
        uint32_t ret = buf[(idx - dind_strt) % bpp];

        kfree(buf);
        kfree(blk_ptrs);

        return ret;
    }
    else
    {
        // TODO: triply indirect blocks
        console_printf("[EXT2] Inode block index too high: %d\n", 255, 0, 0, idx);
        return 0;
    }
}

// TODO: set indirect inode blocks
void ext2_set_inode_blk(ext2_vol_t* vol, uint32_t idx, ext2_inode_t* ino, uint32_t inonum, uint32_t blk)
{
    uint32_t bpp = vol->blk_sz / sizeof(uint32_t);  // Blocks per pointer
    uint32_t sind_strt = EXT2_INO_SIND;             // Singly indirect start
    uint32_t dind_strt = sind_strt + bpp;           // Doubly indirect start
    uint32_t tind_strt = dind_strt + bpp * bpp;     // Triply indirect start

    if (idx < sind_strt)
    {
        ino->blocks[idx] = blk;
        ino->sector_cnt += vol->blk_sz / 512;
        ext2_write_inode(vol, inonum, ino);
    }
    else if (idx < dind_strt)
    {
    }
    else if (idx < tind_strt)
    {
    }
    else
    {
        // TODO: triply indirect blocks
        console_printf("[EXT2] Inode block index too high: %d\n", 255, 0, 0, idx);
    }
}

void ext2_read_inode_blk(ext2_vol_t* vol, uint32_t idx, ext2_inode_t* ino, void* buffer)
{
    vol->dev->read(vol->dev, buffer, ext2_blk_to_lba(vol, ext2_get_inode_blk(vol, idx, ino)), vol->blk_sz / 512);
}

void ext2_write_inode_blk(ext2_vol_t* vol, uint32_t idx, ext2_inode_t* ino, const void* buffer)
{
    vol->dev->write(vol->dev, buffer, ext2_blk_to_lba(vol, ext2_get_inode_blk(vol, idx, ino)), vol->blk_sz / 512);
}

void ext2_alloc_inode_blk(ext2_vol_t* vol, ext2_inode_t* ino, uint32_t inonum)
{
    uint32_t blk = ext2_alloc_block(vol);
    
    uint32_t blk_cnt = ino->sector_cnt / (vol->blk_sz / 512);
    ext2_set_inode_blk(vol, blk_cnt, ino, inonum, blk);
}

void ext2_free_inode_blk(ext2_vol_t* vol, ext2_inode_t* ino, uint32_t inonum, uint32_t blk)
{
    ext2_free_block(vol, blk);

    ext2_set_inode_blk(vol, blk, ino, inonum, 0);
}

uint32_t ext2_alloc_inode(ext2_vol_t* vol)
{
    uint8_t* buf = kmalloc(vol->blk_sz);

    for (uint32_t i = 0; i < vol->blk_grp_cnt; i++)
    {
        if (!vol->blk_grps[i].free_inodes)
            continue;

        uint32_t inode_bm = vol->blk_grps[i].inode_bitmap_addr;
        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, inode_bm), vol->blk_sz / 512);

        for (uint32_t j = 0; j < vol->blk_sz; j++)
        {
            if (buf[j] == 0xff) continue;

            for (uint32_t k = 0; k < 8; k++)
            {
                if ((buf[j] & (1 << k)) == 0)
                {
                    buf[j] |= (1 << k);
                    vol->dev->write(vol->dev, buf, ext2_blk_to_lba(vol, inode_bm), vol->blk_sz / 512);

                    vol->blk_grps[i].free_inodes--;
                    ext2_rewrite_bgds(vol);

                    kfree(buf);
                    return i * vol->super.inodes_per_grp + j * 8 + k;
                }
            }
        }
    }

    console_printf("[EXT2] Warning: could not allocate a free inode!\n", 255, 0, 0);
    kfree(buf);
    return 0;
}

void ext2_free_inode(ext2_vol_t* vol, uint32_t ino)
{

}

void ext2_init_inode(ext2_inode_t* ino, uint32_t type, uint32_t perms)
{
    // Initialize inode structure
    ino->mode = 0;
    ino->mode |= perms & 0xfff;
    ino->mode |= type;
    ino->userid = 0;
    ino->size = 0;
    ino->last_access = 0;
    ino->creation_time = 0; // TODO: now
    ino->last_mod_time = 0;
    ino->del_time = 0;
    ino->grpid = 0;
    ino->link_cnt = type == EXT2_DIR ? 2 : 0;
    ino->sector_cnt = 0;
    ino->flags = 0;
    ino->os_spec1 = 0;
    memset(ino->blocks, 0, 15 * sizeof(uint32_t));
    ino->gen_num = 0;
    ino->size_u = 0;
    ino->frag_addr = 0;
    memset(ino->os_spec2, 0, 12);
}