#include <drivers/fs/ext2/ext2.h>
#include <stdlib.h>
#include <mem/heap.h>

fs_node_t ext2_find_file(fs_vol_t* vol, fs_node_t* dir, const char* name)
{

}

size_t ext2_read(ext2_node_t* node, void* ptr, size_t off, size_t size)
{
    // TEMP
    off = 0;

    uint32_t blk_idx = ext2_loc_to_blk(node->vol, off);
    uint32_t blk_lim = ext2_loc_to_blk(node->vol, off + size);

    uint8_t* buf = kmalloc(node->vol->blk_sz);
    //while (1);

    uint32_t blk_cnt = blk_lim - blk_idx + 1;
    uint32_t* blks = ext2_get_inode_blks(node->vol, blk_idx, blk_cnt, &node->ino);

    for (uint32_t i = 0; i < blk_cnt; i++)
    {
        //node->vol->dev->read(node->vol->dev, ext2_blk_to_lba(node->vol, blks[i]), node->vol->blk_sz / 512, buf);
        node->vol->dev->read(node->vol->dev, 104, 1, buf);

        memcpy(ptr + i * node->vol->blk_sz, buf, size);
    }
    
    kfree(buf);
    kfree(blks);
    
    return size;
}

size_t ext2_fread(fs_node_t* node, void* ptr, size_t off, size_t size)
{
    return ext2_read(node->derived, ptr, off, size);
}