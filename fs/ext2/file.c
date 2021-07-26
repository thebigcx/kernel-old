#include <fs/ext2/ext2.h>
#include <util/stdlib.h>
#include <mem/kheap.h>

size_t ext2_read(vfs_node_t* node, void* ptr, size_t off, size_t size)
{
    ext2_vol_t* vol = (ext2_vol_t*)node->device;

    uint32_t startblk = off / vol->blk_sz;          // Start block
    uint32_t endblk   = (size + off) / vol->blk_sz; // End block
    uint32_t modoff   = off % vol->blk_sz;          // Byte offset of start block
    uint32_t modend   = (size + off) % vol->blk_sz; // Byte offset of end block

    ext2_inode_t ino;
    ext2_read_inode(vol, node->inode_num, &ino);

    uint8_t* buf = kmalloc(vol->blk_sz);
    
    uint64_t ptroff = 0;
    for (uint32_t i = startblk; i <= endblk; i++)
    {
        ext2_read_inode_blk(vol, i, &ino, buf);

        uint32_t start = 0;
        uint32_t size = vol->blk_sz;

        if (i == startblk)
        {
            start = modoff;
            size = vol->blk_sz - start;
        }
        if (i == endblk)
        {
            size = modend;
        }

        memcpy(ptr + ptroff, buf + start, size);

        ptroff += size;
    }

    kfree(buf);
    return size;
}

size_t ext2_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{
    ext2_vol_t* vol = file->device;

    ext2_inode_t ino;
    ext2_read_inode(vol, file->inode_num, &ino);

    if (off + size > file->size + (file->size % vol->blk_sz))
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

    uint32_t startblk = off / vol->blk_sz;          // Start block
    uint32_t endblk   = (size + off) / vol->blk_sz; // End block
    uint32_t modoff   = off % vol->blk_sz;          // Byte offset of start block
    uint32_t modend   = (size + off) % vol->blk_sz; // Byte offset of end block

    uint8_t* buf = kmalloc(vol->blk_sz);
    
    uint64_t ptroff = 0;
    for (uint32_t i = startblk; i <= endblk; i++)
    {
        ext2_read_inode_blk(vol, i, &ino, buf);

        uint32_t start = 0;
        uint32_t size = vol->blk_sz;

        if (i == startblk)
        {
            start = modoff;
            size = vol->blk_sz - start;
        }
        if (i == endblk)
        {
            size = modend;
        }

        memcpy(buf + start, ptr + ptroff, size);

        ext2_write_inode_blk(vol, i, &ino, buf);

        ptroff += size;
    }

    kfree(buf);
    return size;
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