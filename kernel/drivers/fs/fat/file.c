#include <drivers/fs/fat/fat.h>
#include <string.h>
#include <mem/heap.h>
#include <console.h>

fat_node_t fat_get_file(fat_vol_t* dri, fat_node_t* dir, const char* name)
{
    fat_node_t file;

    if (dir != NULL && dir->flags != FAT_DIRECTORY)
    {
        console_write("[FAT] Error: cannot list contents of file (not a directory).\n", 255, 0, 0);
        file.flags = FAT_INVALID;
        return file;
    }

    // Look in root directory if dir is null
    uint32_t cluster = dir != NULL ? dir->cluster : dri->mnt_inf.root_offset;

    uint64_t num_clusters = 0;
    fat_dir_entry_t* dirs = fat_read_cluster_chain(dri, cluster, &num_clusters);

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
                fat_get_lfn(dri, _name, lfn_entries, lfn_cnt);
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
                file.cluster = (((uint32_t)dirs[i].cluster_u) << 16) | dirs[i].cluster;

                if (dirs[i].attr & FAT_ATTR_DIR)
                    file.flags = FAT_DIRECTORY;
                else
                    file.flags = FAT_FILE;

                file.file_len = dirs[i].file_sz;
                file.dri = dri;

                kfree(lfn_entries);
                return file;
            }
        }
    }

    kfree(lfn_entries);
    
    console_write("[FAT] Error: could not find file in directory.\n", 255, 0, 0);
    file.flags = FAT_INVALID;
    return file;
}

size_t fat_file_read(fat_node_t* file, size_t size, size_t off, void* buffer)
{
    uint64_t cnt = 0;
    void* buf = fat_read_cluster_chain(file->dri, file->cluster, &cnt);

    memcpy(buffer, buf + off, size);

    return 1;
}

size_t fat_file_write(fat_node_t* file, size_t size, size_t off, void* buffer)
{
    uint64_t cnt = 0;
    uint32_t* buf = fat_get_cluster_chain(file->dri, file->cluster, &cnt);

    return 1;
}

void fat_write_cluster(fat_vol_t* dri, void* buf, uint32_t size, uint32_t cluster)
{
    uint64_t cnt = 0;
    uint32_t* chain = fat_get_cluster_chain(dri, cluster, &cnt);

    dri->dev->write(dri->dev, fat_cluster_to_lba(dri, chain[0]), 1, buf);
}