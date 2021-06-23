#include <drivers/fs/ext2/ext2.h>
#include <util/stdlib.h>
#include <mem/heap.h>

fs_fd_t* ext2_open(vfs_node_t* file, uint32_t flags)
{
    fs_fd_t* fd = kmalloc(sizeof(fs_fd_t));
    fd->node = file;
    fd->pos = 0;
    fd->flags = flags;
    return fd;
}

size_t ext2_read(vfs_node_t* node, void* ptr, size_t off, size_t size)
{
    ext2_vol_t* vol = (ext2_vol_t*)node->device;
    // TEMP
    off = 0;

    uint32_t blk_idx = ext2_loc_to_blk(vol, off);
    uint32_t blk_lim = ext2_loc_to_blk(vol, off + size);

    uint8_t* buf = kmalloc(vol->blk_sz);

    ext2_inode_t ino;
    ext2_read_inode(vol, node->inode_num, &ino);

    uint32_t blk_cnt = blk_lim - blk_idx + 1;
    uint32_t* blks = ext2_get_inode_blks(vol, blk_idx, blk_cnt, &ino);

    uint32_t blk_off = 0;

    for (uint32_t i = 0; i < blk_cnt; i++)
    {
        blk_off = i * vol->blk_sz;
        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, blks[i]), vol->blk_sz / 512);

        memcpy(ptr + blk_off, buf, vol->blk_sz);
    }
    
    kfree(buf);
    kfree(blks);
    
    return size;
}

size_t ext2_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{

}

void ext2_close(vfs_node_t* file)
{

}

size_t ext2_get_size(ext2_inode_t* ino)
{
    return (uint64_t)ino->size + ((uint64_t)ino->size_u << 32);
}