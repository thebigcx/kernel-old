#include <drivers/fs/ext2/ext2.h>

void ext2_write_inode(ext2_superblock_t* sb, ext2_inode_t* inode, uint32_t idx, dev_t* dev)
{
    uint32_t group = idx / sb->inodes_per_group;
}