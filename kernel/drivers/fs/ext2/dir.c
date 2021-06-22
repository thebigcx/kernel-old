#include <drivers/fs/ext2/ext2.h>
#include <util/stdlib.h>
#include <mem/heap.h>

fs_node_t ext2_finddir(fs_vol_t* vol, fs_node_t* node, const char* name)
{
    ext2_vol_t* e2vol = (ext2_vol_t*)vol->derived;

    ext2_inode_t ino;
    ext2_read_inode(e2vol, node ? ((ext2_node_t*)node->derived)->ino : 2, &ino);

    uint32_t blk_idx = 0;
    uint32_t blk_off = 0;
    uint8_t* buf = kmalloc(e2vol->blk_sz);
    ext2_dir_ent_t* fnd_dir = buf; // Found directory entry

    e2vol->dev->read(e2vol->dev, ext2_blk_to_lba(e2vol, ext2_get_inode_blk(e2vol, blk_idx, &ino)), e2vol->blk_sz / 512, buf);

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

            e2vol->dev->read(e2vol->dev, ext2_blk_to_lba(e2vol, ext2_get_inode_blk(e2vol, blk_idx, &ino)), e2vol->blk_sz / 512, buf);
        }

        fnd_dir = buf + blk_off;
    }

    kfree(buf);
}

fs_node_t* ext2_read_dir(ext2_vol_t* vol, fs_node_t* parent, uint32_t* cnt)
{
    *cnt = 0;

    ext2_inode_t ino;
    ext2_read_inode(vol, parent ? ((ext2_node_t*)parent->derived)->ino : 2, &ino);

    fs_node_t* nodes = kmalloc(sizeof(fs_node_t) * 10);

    uint32_t blk_idx = 0;
    uint32_t blk_off = 0;
    uint8_t* buf = kmalloc(vol->blk_sz);
    ext2_dir_ent_t* fnd_dir = buf; // Found directory entry

    vol->dev->read(vol->dev, ext2_blk_to_lba(vol, ext2_get_inode_blk(vol, blk_idx, &ino)), vol->blk_sz / 512, buf);

    while (blk_idx < ino.sector_cnt / (vol->blk_sz / 512))
    {
        blk_off += fnd_dir->size;

        nodes[(*cnt)++] = ext2_dirent_to_node(vol, fnd_dir);

        if (blk_off >= vol->blk_sz)
        {
            blk_idx++;
            blk_off = 0;

            vol->dev->read(vol->dev, ext2_blk_to_lba(vol, ext2_get_inode_blk(vol, blk_idx, &ino)), vol->blk_sz / 512, buf);
        }

        fnd_dir = buf + blk_off;
    }

    kfree(buf);

    return nodes;
}

fs_node_t ext2_dirent_to_node(ext2_vol_t* vol, ext2_dir_ent_t* dirent)
{
    fs_node_t node;

    node.read = ext2_read;
    node.write = ext2_write;
    node.open = ext2_open;
    node.close = ext2_close;

    node.derived = kmalloc(sizeof(ext2_node_t));

    node.name = kmalloc(dirent->name_len + 1);
    strncpy(node.name, (void*)dirent + EXT2_DIRENT_NAME_OFF, dirent->name_len);
    node.name[dirent->name_len] = 0;

    ext2_inode_t ino;
    ext2_read_inode(vol, dirent->inode, &ino);
    node.size = ext2_get_size(&ino);

    ((ext2_node_t*)node.derived)->ino = dirent->inode;
    ((ext2_node_t*)node.derived)->vol = vol;

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