#include <drivers/fs/ext2/ext2.h>
#include <util/stdlib.h>
#include <mem/heap.h>

fs_node_t ext2_find_file(fs_vol_t* vol, fs_node_t* dir, const char* name)
{
    ext2_node_t* parent = dir ? dir->derived : NULL;
    ext2_node_t* enode = kmalloc(sizeof(ext2_node_t));

    fs_node_t node;
    node.open = ext2_fopen;
    node.close = ext2_fclose;
    node.read = ext2_fread;
    node.write = ext2_fwrite;
    node.get_size = ext2_fget_size;

    node.derived = (void*)enode;
    *enode = ext2_find_dir((ext2_vol_t*)vol->derived, parent, name);

    return node;
}

size_t ext2_read(ext2_node_t* node, void* ptr, size_t off, size_t size)
{
    // TEMP
    off = 0;

    uint32_t blk_idx = ext2_loc_to_blk(node->vol, off);
    uint32_t blk_lim = ext2_loc_to_blk(node->vol, off + size);

    uint8_t* buf = kmalloc(node->vol->blk_sz);

    uint32_t blk_cnt = blk_lim - blk_idx + 1;
    uint32_t* blks = ext2_get_inode_blks(node->vol, blk_idx, blk_cnt, &node->ino);

    for (uint32_t i = 0; i < blk_cnt; i++)
    {
        node->vol->dev->read(node->vol->dev, ext2_blk_to_lba(node->vol, blks[i]), node->vol->blk_sz / 512, buf);

        memcpy(ptr + i * node->vol->blk_sz, buf, size);
    }
    
    kfree(buf);
    kfree(blks);
    
    return size;
}

fs_fd_t* ext2_fopen(fs_node_t* file, uint32_t flags)
{
    fs_fd_t* fd = kmalloc(sizeof(fs_fd_t));
    fd->node = file;
    fd->pos = 0;
    fd->flags = flags;
    return fd;
}

size_t ext2_fread(fs_node_t* node, void* ptr, size_t off, size_t size)
{
    return ext2_read(node->derived, ptr, off, size);
}

size_t ext2_fwrite(fs_node_t* file, const void* ptr, size_t off, size_t size)
{

}

void ext2_fclose(fs_node_t* file)
{

}

size_t ext2_fget_size(fs_node_t* file)
{
    ext2_inode_t* inode = &((ext2_node_t*)file->derived)->ino;
    return (uint64_t)inode->size + ((uint64_t)inode->size_u << 32);
}