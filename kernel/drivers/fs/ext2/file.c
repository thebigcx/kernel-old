#include <drivers/fs/ext2/ext2.h>
#include <util/stdlib.h>
#include <mem/heap.h>

void ext2_open(vfs_node_t* file, uint32_t flags)
{
    
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
    ext2_vol_t* vol = file->device;

    ext2_inode_t ino;
    ext2_read_inode(vol, file->inode_num, &ino);

    if (off + size > file->size)
    {
        file->size = off + size;

        for (uint32_t i = 0; i < (off + size - file->size) / vol->blk_sz + 1; i++)
            ext2_alloc_inode_blk(vol, &ino, file->inode_num);

        ino.size = off + size;
        ext2_write_inode(vol, file->inode_num, &ino);
    }
    else if (off + size < file->size - (vol->blk_sz - 1))
    {
        // Free blocks
    }

    uint32_t start_blk = off / vol->blk_sz;
    uint32_t end_blk = (off + size) / vol->blk_sz;

    uint8_t* buf = kmalloc(vol->blk_sz);

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
    ext2_add_entry(parent, name, EXT2_REGFILE);
}

void ext2_mkdir(vfs_node_t* parent, const char* name)
{
    ext2_add_entry(parent, name, EXT2_DIR);
}