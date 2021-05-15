#include <drivers/fs/fat/fat.h>
#include <string.h>

void fat_read_dir(fat_dri_t* dri, uint32_t cluster, fat_file_t* files, uint32_t* cnt)
{
    uint64_t numclus = 0;

    fat_dir_entry_t* dirs = fat_read_cluster_chain(dri, cluster, &numclus);

    for (uint32_t i = 0; i < numclus * 512 / sizeof(fat_dir_entry_t); i++)
    {
        if (dirs[i].name[0] == '\0') continue; // Empty

        strcpy(files[*cnt].name, dirs[i].name);
        strcpy(files[*cnt].name + 8, dirs[i].ext);
        files[*cnt].name[11] = '\0';
        files[*cnt].curr_cluster = (((uint32_t)dirs[i].cluster_u) << 16) | dirs[i].cluster;
        files[*cnt].file_len = dirs[i].file_sz;

        files[*cnt].flags = dirs[i].attrib & FAT_ATTR_DIR ? FAT_DIRECTORY : FAT_FILE;
        
        (*cnt)++;
    }
}