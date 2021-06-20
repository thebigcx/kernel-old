#include <drivers/fs/ext2/ext2.h>
#include <util/stdlib.h>

ext2_node_t ext2_find_dir(ext2_vol_t* vol, ext2_node_t* node, const char* name)
{
    ext2_inode_t ino;
    if (node)
        ino = node->ino;
    else
        ext2_read_inode(vol, 2, &ino); // Root directory

    uint32_t blk_idx = 0;
    uint32_t blk_off = 0;
    uint8_t* buf = kmalloc(vol->blk_sz);
    ext2_dir_ent_t* fnd_dir = buf; // Found directory entry

    vol->dev->read(vol->dev, ext2_blk_to_lba(vol, ext2_get_inode_blk(vol, blk_idx, &ino)), vol->blk_sz / 512, buf);

    while (blk_idx < ino.sector_cnt / (vol->blk_sz / 512))
    {
        char* fnd_name = (uint8_t*)fnd_dir + EXT2_DIRENT_NAME_OFF;
        blk_off += fnd_dir->size;

        if (strncmp(fnd_name, name, fnd_dir->name_len) == 0)
        {
            ext2_node_t retnode;

            ext2_read_inode(vol, fnd_dir->inode, &retnode.ino);
            retnode.name = kmalloc(fnd_dir->name_len + 1);
            strncpy(retnode.name, fnd_name, fnd_dir->name_len);

            retnode.name[fnd_dir->name_len] = 0; // Null terminate
            retnode.type = fnd_dir->file_type;
            retnode.vol = vol;

            return retnode;
        }

        if (blk_off >= vol->blk_sz)
        {
            blk_idx++;
            blk_off = 0;

            vol->dev->read(vol->dev, ext2_blk_to_lba(vol, ext2_get_inode_blk(vol, blk_idx, &ino)), vol->blk_sz / 512, buf);
        }

        fnd_dir = buf + blk_off;
    }

    kfree(buf);
    kfree(ino);
}