#include <drivers/fs/fat/fat.h>
#include <string.h>

void to_dos_filename(char* dst, const char* src)
{
    strcpy(dst, src);
}

fat_file_t fat_get_file(fat_dri_t* dri, fat_file_t* dir, const char* name)
{
    fat_file_t file;
    // Look in root directory if dir is null
    uint32_t cluster = dir != NULL ? dir->curr_cluster : dri->mnt_inf.root_offset;

    char dos_filename[12];
    to_dos_filename(dos_filename, name);
    dos_filename[11] = '\0';

    uint64_t num_clusters = 0;
    fat_dir_entry_t* dirs = fat_read_cluster_chain(dri, cluster, &num_clusters);

    for (uint32_t i = 0; i < num_clusters * 512; i++)
    {
        char name[12];
        strcpy(name, dirs[i].name);
        strcpy(name + 8, dirs[i].ext);
        name[11] = '\0';

        if (strcmp(dos_filename, name) == 0)
        {
            strcpy(file.name, name);
            file.curr_cluster = (((uint32_t)dirs[i].cluster_u) << 16) | dirs[i].cluster;

            if (dirs[i].attrib & FAT_ATTR_DIR)
                file.flags = FAT_DIRECTORY;
            else
                file.flags = FAT_FILE;

            file.file_len = dirs[i].file_sz;

            return file;
        }
    }
    
    file.flags = FAT_INVALID;
    return file;
}

void fat_file_read(fat_dri_t* dri, fat_file_t* file, void* buffer)
{
    /*uint32_t phys_sector = 32 + (file->curr_cluster - 1);
    
    uint8_t* sector;
    dri->dev->read(dri->dev, phys_sector, 1, buffer);
    memcpy(buffer, sector, 512);

    uint32_t fat_off = file->curr_cluster * 4;
    uint32_t fat_sector = 1 + (fat_off / 512);
    uint32_t entry_off = fat_off % 512;
    
    uint8_t* fat = page_request();
    dri->dev->read(dri->dev, fat_sector, 2, fat);

    uint32_t next_cluster = *(uint32_t*)&fat[entry_off];

    if (next_cluster >= 0xff8)
    {
        file->eof = 1;
        return;
    }

    file->curr_cluster = next_cluster;*/

    uint64_t cnt = 0;
    void* buf = fat_read_cluster_chain(dri, file->curr_cluster, &cnt);

    memcpy(buffer, buf, 512);
}