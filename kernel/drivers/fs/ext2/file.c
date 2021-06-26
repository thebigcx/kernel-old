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

    uint32_t start_blk = off / vol->blk_sz;
    uint32_t end_blk = (off + size) / vol->blk_sz;

    uint8_t* buf = kmalloc(vol->blk_sz);

    ext2_inode_t ino;
    ext2_read_inode(vol, node->inode_num, &ino);

    uint32_t blk_cnt = end_blk - start_blk + 1;
    uint32_t blk_off = 0;
    for (uint32_t i = 0; i < blk_cnt; i++)
    {
        blk_off = i * vol->blk_sz;

        uint32_t blk = ext2_get_inode_blk(vol, i, &ino);
        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, blk), vol->blk_sz / 512);

        memcpy(ptr + blk_off, buf, vol->blk_sz);
    }
    
    kfree(buf);
    
    return size;
}

size_t ext2_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{
    if (off + size > file->size)
    {
        // Resize inode
        
    }

    ext2_vol_t* vol = (ext2_vol_t*)file->device;

    uint32_t start_blk = off / vol->blk_sz;
    uint32_t end_blk = (off + size) / vol->blk_sz;

    uint8_t* buf = kmalloc(vol->blk_sz);

    ext2_inode_t ino;
    ext2_read_inode(vol, file->inode_num, &ino);

    uint32_t cnt = end_blk - start_blk + 1;
    for (uint32_t i = 0; i < cnt; i++)
    {
        uint32_t blk_off = i * vol->blk_sz;

        uint32_t blk = ext2_get_inode_blk(vol, i, &ino);

        vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, blk), vol->blk_sz / 512);
        memcpy(buf, ptr + blk_off, vol->blk_sz);
        vol->dev->write(vol->dev, buf, ext2_blk_to_lba(vol, blk), vol->blk_sz / 512);
    }

    kfree(buf);

    return size;
}

void ext2_close(vfs_node_t* file)
{

}

size_t ext2_get_size(ext2_inode_t* ino)
{
    return (uint64_t)ino->size + ((uint64_t)ino->size_u << 32);
}

void ext2_mkfile(vfs_node_t* parent, const char* name)
{

}

void ext2_mkdir(vfs_node_t* parent, const char* name)
{

}