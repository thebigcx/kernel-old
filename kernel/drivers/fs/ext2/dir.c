#include <drivers/fs/ext2/ext2.h>
#include <util/stdlib.h>
#include <mem/heap.h>

vfs_node_t ext2_finddir(vfs_node_t* dir, const char* name)
{
    ext2_vol_t* e2vol = (ext2_vol_t*)dir->device;

    ext2_inode_t ino;
    ext2_read_inode(e2vol, dir ? dir->inode_num : 2, &ino);

    uint32_t blk_idx = 0;
    uint32_t blk_off = 0;
    uint8_t* buf = kmalloc(e2vol->blk_sz);
    ext2_dir_ent_t* fnd_dir = buf; // Found directory entry

    e2vol->dev->read(e2vol->dev, buf, ext2_blk_to_lba(e2vol, ext2_get_inode_blk(e2vol, blk_idx, &ino)), e2vol->blk_sz / 512);

    while (blk_idx < ino.sector_cnt / (e2vol->blk_sz / 512))
    {
        char* fnd_name = (uint8_t*)fnd_dir + EXT2_DIRENT_NAME_OFF;
        blk_off += fnd_dir->size;

        if (strncmp(fnd_name, name, fnd_dir->name_len) == 0)
        {
            return ext2_dirent_to_node(e2vol, fnd_dir);
        }

        if (blk_off >= e2vol->blk_sz)
        {
            blk_idx++;
            blk_off = 0;

            e2vol->dev->read(e2vol->dev, buf, ext2_blk_to_lba(e2vol, ext2_get_inode_blk(e2vol, blk_idx, &ino)), e2vol->blk_sz / 512);
        }

        fnd_dir = buf + blk_off;
    }

    kfree(buf);
}

list_t* ext2_read_dir(ext2_vol_t* vol, vfs_node_t* parent)
{
    ext2_inode_t ino;
    ext2_read_inode(vol, parent ? parent->inode_num : 2, &ino);

    list_t* nodes = list_create();

    uint32_t blk_idx = 0;
    uint32_t blk_off = 0;
    uint8_t* buf = kmalloc(vol->blk_sz);
    ext2_dir_ent_t* fnd_dir = buf; // Found directory entry

    vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, ext2_get_inode_blk(vol, blk_idx, &ino)), vol->blk_sz / 512);

    while (blk_idx < ino.sector_cnt / (vol->blk_sz / 512))
    {
        blk_off += fnd_dir->size;

        vfs_node_t* node = kmalloc(sizeof(vfs_node_t));
        *node = ext2_dirent_to_node(vol, fnd_dir);
        list_push_back(nodes, node);

        if (blk_off >= vol->blk_sz)
        {
            blk_idx++;
            blk_off = 0;

            vol->dev->read(vol->dev, buf, ext2_blk_to_lba(vol, ext2_get_inode_blk(vol, blk_idx, &ino)), vol->blk_sz / 512);
        }

        fnd_dir = buf + blk_off;
    }

    kfree(buf);

    return nodes;
}

vfs_node_t ext2_dirent_to_node(ext2_vol_t* vol, ext2_dir_ent_t* dirent)
{
    vfs_node_t node;

    node.read = ext2_read;
    node.write = ext2_write;
    node.open = ext2_open;
    node.close = ext2_close;
    node.finddir = ext2_finddir;

    node.device = vol;

    node.name = kmalloc(dirent->name_len + 1);
    strncpy(node.name, (void*)dirent + EXT2_DIRENT_NAME_OFF, dirent->name_len);
    node.name[dirent->name_len] = 0;

    ext2_inode_t ino;
    ext2_read_inode(vol, dirent->inode, &ino);
    node.size = ext2_get_size(&ino);

    node.inode_num = dirent->inode;

    if (dirent->file_type == EXT2_REGFILE)
    {
        node.flags = FS_FILE;
    }
    if (dirent->file_type == EXT2_DIR)
    {
        node.flags = FS_DIR;
    }

    return node;
}