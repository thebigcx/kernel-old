#include <drivers/fs/fat/fat.h>
#include <string.h>

fat_file_t fat_get_file(fat_dri_t* dri, fat_file_t* dir, const char* name)
{
    fat_file_t file;

    if (dir->flags != FAT_DIRECTORY)
    {
        puts("[FAT] Error: cannot list contents of file (not a directory).\n");
        file.flags = FAT_INVALID;
        return file;
    }

    // Look in root directory if dir is null
    uint32_t cluster = dir != NULL ? dir->curr_cluster : dri->mnt_inf.root_offset;

    uint64_t num_clusters = 0;
    fat_dir_entry_t* dirs = fat_read_cluster_chain(dri, cluster, &num_clusters);

    fat_lfn_entry_t** lfn_entries = malloc(sizeof(fat_lfn_entry_t*) * 10);
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
        else if (dirs[i].attrib == FAT_ATTR_LFN) // Long file entry
        {
            lfn_entries[lfn_cnt++] = (fat_lfn_entry_t*)&dirs[i];
        }
        else if (dirs[i].attrib & FAT_ATTR_VOL_ID)
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
                strcpy(_name, dirs[i].name);
                strcpy(_name + 8, dirs[i].ext);
                _name[11] = '\0';
            }

            if (strcmp(name, _name) == 0)
            {
                strcpy(file.name, _name);
                file.curr_cluster = (((uint32_t)dirs[i].cluster_u) << 16) | dirs[i].cluster;

                if (dirs[i].attrib & FAT_ATTR_DIR)
                    file.flags = FAT_DIRECTORY;
                else
                    file.flags = FAT_FILE;

                file.file_len = dirs[i].file_sz;

                return file;
            }
        }
    }

    free(lfn_entries);
    
    puts("[FAT] Error: could not find file in directory.\n");
    file.flags = FAT_INVALID;
    return file;
}

void fat_file_read(fat_dri_t* dri, fat_file_t* file, size_t size, size_t off, void* buffer)
{
    uint64_t cnt = 0;
    void* buf = fat_read_cluster_chain(dri, file->curr_cluster, &cnt);

    memcpy(buffer, buf + off, size);
}

void fat_file_write(fat_dri_t* dri, fat_file_t* file, size_t size, size_t off, void* buffer)
{
    uint64_t cnt = 0;
    uint32_t* buf = fat_get_cluster_chain(dri, file->curr_cluster, &cnt);
}

void fat_write_cluster(fat_dri_t* dri, void* buf, uint32_t size, uint32_t cluster)
{
    uint64_t cnt = 0;
    uint32_t* chain = fat_get_cluster_chain(dri, cluster, &cnt);

    dri->dev->write(dri->dev, fat_cluster_to_lba(dri, chain[0]), 1, buf);
}