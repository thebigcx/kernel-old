#include <drivers/fs/ext2/ext2.h>
#include <util/stdlib.h>
#include <mem/heap.h>

vfs_node_t* ext2_finddir(vfs_node_t* dir, const char* name)
{
    ext2_vol_t* e2vol = (ext2_vol_t*)dir->device;

    ext2_inode_t ino;
    ext2_read_inode(e2vol, dir ? dir->inode_num : 2, &ino);

    uint32_t blk_idx = 0;
    uint32_t blk_off = 0;
    uint8_t* buf = kmalloc(e2vol->blk_sz);
    ext2_dir_ent_t* fnd_dir = buf; // Found directory entry

    ext2_read_inode_blk(e2vol, blk_idx, &ino, buf);

    while (blk_idx * e2vol->blk_sz < ino.size)
    {
        char* fnd_name = (uint8_t*)fnd_dir + EXT2_DIRENT_NAME_OFF;
        blk_off += fnd_dir->size;

        if (strncmp(fnd_name, name, fnd_dir->name_len) == 0)
        {
            vfs_node_t* node = ext2_dirent_to_node(e2vol, fnd_dir);
            kfree(buf);
            return node;
        }

        if (blk_off >= e2vol->blk_sz)
        {
            blk_idx++;
            blk_off = 0;

            ext2_read_inode_blk(e2vol, blk_idx, &ino, buf);
        }

        fnd_dir = buf + blk_off;
    }

    console_printf("[EXT2] Could not find file %s\n", 255, 255, 255, name);
    kfree(buf);
    return NULL;
}

list_t* ext2_listdir(vfs_node_t* parent)
{
    ext2_vol_t* vol = (ext2_vol_t*)parent->device;

    ext2_inode_t ino;
    ext2_read_inode(vol, parent ? parent->inode_num : 2, &ino);

    list_t* nodes = list_create();

    uint32_t blk_idx = 0;
    uint32_t blk_off = 0;
    uint8_t* buf = kmalloc(vol->blk_sz);
    ext2_dir_ent_t* fnd_dir = buf; // Found directory entry

    ext2_read_inode_blk(vol, blk_idx, &ino, buf);

    while (blk_idx * vol->blk_sz < ino.size)
    {
        blk_off += fnd_dir->size;

        vfs_node_t* node = ext2_dirent_to_node(vol, fnd_dir);
        list_push_back(nodes, node);

        if (blk_off >= vol->blk_sz)
        {
            blk_idx++;
            blk_off = 0;

            ext2_read_inode_blk(vol, blk_idx, &ino, buf);
        }

        fnd_dir = buf + blk_off;
    }

    kfree(buf);

    return nodes;
}

vfs_node_t* ext2_dirent_to_node(ext2_vol_t* vol, ext2_dir_ent_t* dirent)
{
    vfs_node_t* node = kmalloc(sizeof(vfs_node_t));

    node->read = ext2_read;
    node->write = ext2_write;
    node->open = ext2_open;
    node->close = ext2_close;
    node->finddir = ext2_finddir;
    node->listdir = ext2_listdir;
    node->mkfile = ext2_mkfile;
    node->mkdir = ext2_mkdir;

    node->device = vol;

    node->name = kmalloc(dirent->name_len + 1);
    strncpy(node->name, (void*)dirent + EXT2_DIRENT_NAME_OFF, dirent->name_len);
    node->name[dirent->name_len] = 0;

    ext2_inode_t ino;
    ext2_read_inode(vol, dirent->inode, &ino);
    node->size = ext2_get_size(&ino);

    node->inode_num = dirent->inode;

    if (dirent->file_type == EXT2_REGFILE)
    {
        node->flags = FS_FILE;
    }
    if (dirent->file_type == EXT2_DIR)
    {
        node->flags = FS_DIR;
    }

    return node;
}

void ext2_add_entry(vfs_node_t* parent, const char* name, uint32_t type)
{
    ext2_vol_t* vol = (ext2_vol_t*)parent->device;

    uint32_t newino_num = ext2_alloc_inode(vol);
    ext2_inode_t newino;
    ext2_init_inode(&newino, type, 0); // TODO: permissions
    ext2_write_inode(vol, newino_num, &newino);

    uint32_t size = sizeof(ext2_dir_ent_t) + strlen(name);
    ext2_dir_ent_t* dirent = kmalloc(size);
    dirent->file_type = type;
    dirent->inode = newino_num;
    dirent->name_len = strlen(name);
    dirent->size = size;
    memcpy((void*)dirent + sizeof(ext2_dir_ent_t), name, dirent->name_len);

    uint32_t pinonum = parent ? parent->inode_num : 2; // Root directory if NULL
    ext2_inode_t pino;
    ext2_read_inode(vol, pinonum, &pino);
    size_t inosize = ext2_get_size(&pino);

    uint32_t blk_off = 0;
    uint32_t blk_idx = 0;
    uint32_t direntoff = 0;
    void* buf = kmalloc(vol->blk_sz);
    ext2_dir_ent_t* ent = buf;

    ext2_read_inode_blk(vol, blk_idx, &pino, buf);

    while (blk_idx * vol->blk_sz < pino.size)
    {
        blk_off += ent->size;

        if (blk_off >= vol->blk_sz)
        {
            blk_idx++;
            blk_off = 0;
            
            ext2_read_inode_blk(vol, blk_idx, &pino, buf);
        }

        // Last entry - link_cnt excludes . and ..
        if (direntoff == pino.link_cnt + 1)
        {
            //ent = buf + blk_off;
            // Set the size to the actual size (not the rest of the block)
            ent->size = ent->name_len + sizeof(ext2_dir_ent_t);

            blk_off += ent->size;
            ent = buf + blk_off;
            break;
        }

        ent = buf + blk_off;
        direntoff++;
    }

    memcpy(ent, dirent, dirent->size);
    ext2_write_inode_blk(vol, blk_idx, &pino, buf);

    pino.link_cnt++;
    ext2_write_inode(vol, pinonum, &pino);

    kfree(dirent);
}