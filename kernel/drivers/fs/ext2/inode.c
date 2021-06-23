#include <drivers/fs/ext2/ext2.h>

#define EXT2_INO_SIND 12 // Singly indirect
#define EXT2_INO_DIND 13 // Doubly indirect
#define EXT2_INO_TIND 14 // Triply indirect

void ext2_write_inode(ext2_vol_t* vol, ext2_inode_t* inode, uint32_t idx)
{
    uint32_t group = idx / vol->super.inode_cnt;
}

void ext2_read_inode(ext2_vol_t* vol, uint32_t num, ext2_inode_t* inode)
{
    uint8_t* buf = kmalloc(vol->blk_sz);
    uint64_t lba = ext2_inode_lba(vol, num);

    vol->dev->read(vol->dev, buf, lba, 1);

    memcpy(inode, (buf + (ext2_inode_bg_idx(vol, num) * vol->superext.inode_sz) % 512), sizeof(ext2_inode_t));
    kfree(buf);
}

uint32_t* ext2_get_inode_blks(ext2_vol_t* vol, uint32_t idx, uint32_t cnt, ext2_inode_t* ino)
{
    uint32_t* ret = kmalloc(cnt * sizeof(uint32_t));

    uint32_t bpp = vol->blk_sz / sizeof(uint32_t);  // Blocks per pointer
    uint32_t sind_strt = EXT2_INO_SIND;             // Singly indirect start
    uint32_t dind_strt = sind_strt + bpp;           // Doubly indirect start
    uint32_t tind_strt = dind_strt + bpp * bpp;     // Triply indirect start

    uint32_t i = idx;
    uint32_t lst_cnt = 0;
    while (i < sind_strt && i < idx + cnt)
    {
        ret[lst_cnt++] = ino->blocks[i++];
    }

    if (i < dind_strt && i < idx + cnt)
    {
        uint32_t* buf = kmalloc(vol->blk_sz);

        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, ino->blocks[EXT2_INO_SIND]), vol->blk_sz / 512);

        while (i < dind_strt && i < idx + cnt)
        {
            ret[lst_cnt++] = buf[i - sind_strt];
            i++;
        }

        kfree(buf);
    }
    
    if (i < tind_strt && i < idx + cnt)
    {
        uint32_t* blk_ptrs = kmalloc(vol->blk_sz);
        uint32_t* buf = kmalloc(vol->blk_sz);

        vol->dev->read(vol->dev, blk_ptrs, ext2_blk_to_lba(vol, ino->blocks[EXT2_INO_DIND]), vol->blk_sz / 512);

        while (i < tind_strt && i < idx + cnt)
        {
            uint32_t blk_ptr = blk_ptrs[(i - dind_strt) / bpp];

            vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, blk_ptr), vol->blk_sz / 512);

            uint32_t blk_ptr_cnt = i + (bpp - (i - dind_strt) % bpp);
            while (i < blk_ptr_cnt && i < idx + cnt)
            {
                ret[lst_cnt++] = buf[(i - dind_strt) % bpp];
                i++;
            }
        }

        kfree(buf);
        kfree(blk_ptrs);
    }

    if (i < idx + cnt)
    {
        // TODO: triply indirect blocks
        console_printf("[EXT2] Index %d and count %d too high\n", 255, 0, 0, idx, cnt);
        return NULL;
    }

    return ret;
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
        uint32_t* buf = kmalloc(vol->blk_sz / sizeof(uint32_t));

        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, ino->blocks[EXT2_INO_SIND]), vol->blk_sz / 512);

        uint32_t ret = buf[idx - EXT2_INO_SIND];
        kfree(buf);
        return ret;
    }
    else if (idx < tind_strt)
    {
        uint32_t* blk_ptrs = kmalloc(vol->blk_sz / sizeof(uint32_t));
        uint32_t* buf = kmalloc(vol->blk_sz / sizeof(uint32_t));

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