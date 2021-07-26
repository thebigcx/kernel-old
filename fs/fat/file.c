#include <fs/fat/fat.h>
#include <util/stdlib.h>
#include <mem/kheap.h>
#include <sys/console.h>

void fat_write_cluster(fat_vol_t* vol, void* buf, uint32_t size, uint32_t cluster)
{
    uint64_t cnt = 0;
    uint32_t* chain = fat_get_cluster_chain(vol, cluster, &cnt);

    vol->dev->write(vol->dev, fat_cluster_to_lba(vol, chain[0]), 1, buf);
}

size_t fat_read(vfs_node_t* file, void* ptr, size_t off, size_t size)
{
    uint64_t cnt = 0;
    void* buf = fat_read_cluster_chain((fat_vol_t*)file->device, file->inode_num, &cnt);

    memcpy(ptr, buf + off, size);

    return 1;
}

size_t fat_write(vfs_node_t* file, const void* ptr, size_t off, size_t size)
{

}

vfs_node_t fat_finddir(vfs_node_t* dir, const char* name)
{
    fat_vol_t* vol = (fat_vol_t*)dir->device;
    vfs_node_t file;

    if (dir != NULL && dir->flags != FAT_DIRECTORY)
    {
        console_printf("[FAT32] Cannot list contents of \"%s\" (not a directory)", 255, 0, 0, dir->name);
        file.flags = FAT_INVALID;
        return file;
    }

    // Look in root directory if dir is null
    uint32_t cluster = dir != NULL ? dir->inode_num : vol->mnt_inf.root_offset;

    uint64_t num_clusters = 0;
    fat_dir_entry_t* dirs = fat_read_cluster_chain(vol, cluster, &num_clusters);

    fat_lfn_entry_t** lfn_entries = kmalloc(sizeof(fat_lfn_entry_t*) * 10);
    uint32_t lfn_cnt = 0;

    for (uint32_t i = 0; i < num_clusters * 512; i++)
    {
        if (dirs[i].name[0] == '\0') // Empty
        {
            continue;
        }
        else if (dirs[i].name[0] == 0xe5)
        {
            lfn_cnt = 0;
            continue;
        }
        else if (dirs[i].attr == FAT_ATTR_LFN) // Long file entry
        {
            lfn_entries[lfn_cnt++] = (fat_lfn_entry_t*)&dirs[i];
        }
        else if (dirs[i].attr & FAT_ATTR_VOL_ID)
        {
            lfn_cnt = 0;
            continue;
        }
        else
        {
            char _name[128];

            if (lfn_cnt)
            {
                fat_get_lfn(vol, _name, lfn_entries, lfn_cnt);
            }
            else
            {
                strcpy(_name, (char*)dirs[i].name);
                strcpy(_name + 8, (char*)dirs[i].ext);
                _name[11] = '\0';
            }

            if (strcmp(name, _name) == 0)
            {
                strcpy(file.name, _name);
                file.inode_num = (((uint32_t)dirs[i].cluster_u) << 16) | dirs[i].cluster;

                if (dirs[i].attr & FAT_ATTR_DIR)
                    file.flags = FAT_DIRECTORY;
                else
                    file.flags = FAT_FILE;

                file.size = dirs[i].file_sz;
                file.device = vol;
                file.read = fat_read;
                file.write = fat_write;
                file.finddir = fat_finddir;

                kfree(lfn_entries);
                return file;
            }
        }
    }

    kfree(lfn_entries);
    
    console_printf("[FAT32] Could not find file \"%s\" in directory \"%s\"\n", 255, 0, 0, name, dir->name);
    file.flags = FAT_INVALID;
    return file;
}