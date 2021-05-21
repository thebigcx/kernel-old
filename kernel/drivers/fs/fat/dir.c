#include <drivers/fs/fat/fat.h>
#include <string.h>
#include <stdlib.h>

void fat_read_dir(fat_dri_t* dri, uint32_t cluster, fat_file_t* files, uint32_t* cnt)
{
    uint64_t numclus = 0;

    fat_dir_entry_t* dirs = fat_read_cluster_chain(dri, cluster, &numclus);

    fat_lfn_entry_t** lfn_entries = malloc(sizeof(fat_lfn_entry_t*) * 10);
    uint32_t lfn_cnt = 0;

    for (uint32_t i = 0; i < numclus * 512 / sizeof(fat_dir_entry_t); i++)
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
            if (lfn_cnt)
            {
                fat_get_lfn(dri, files[*cnt].name, lfn_entries, lfn_cnt);
            }
            else
            {
                strcpy(files[*cnt].name, dirs[i].name);
                strcpy(files[*cnt].name + 8, dirs[i].ext);
                files[*cnt].name[11] = '\0';
            }

            files[*cnt].curr_cluster = (((uint32_t)dirs[i].cluster_u) << 16) | dirs[i].cluster;
            files[*cnt].file_len = dirs[i].file_sz;

            files[*cnt].flags = dirs[i].attrib & FAT_ATTR_DIR ? FAT_DIRECTORY : FAT_FILE;
            
            (*cnt)++;
        }
    }

    free(lfn_entries);
}

void strsplit(char** arr, const char* str, char c, uint32_t* cnt)
{
    size_t len = strlen(str);
    int arr_len = 0;

    char* token = malloc(50);
    int tok_size = 0;
    for (size_t i = 0; i < len + 1; i++)
    {
        if (str[i] == c || str[i] == '\0')
        {
            if (tok_size)
            {
                token[tok_size] = '\0';
                strcpy(arr[arr_len++], token);
                tok_size = 0;
            }
            continue;
        }

        token[tok_size++] = str[i];
    }

    *cnt = arr_len;
}

fat_file_t fat_traverse_path(fat_dri_t* dri, const char* path)
{
    char* parts[10];
    for (int i = 0; i < 10; i++)
    {
        parts[i] = malloc(32);
    }

    uint32_t part_cnt;

    strsplit(parts, path, '/', &part_cnt);

    fat_file_t file;

    for (uint32_t i = 0; i < part_cnt; i++)
    {
        if (i == 0)
        {
            file = fat_get_file(dri, NULL, parts[i]);
            if (file.flags == FAT_INVALID)
            {
                puts("[FAT] Could not traverse path (file does not exist).\n");
                return file;
            }
               
            continue;
        }

        file = fat_get_file(dri, &file, parts[i]);
        if (file.flags == FAT_INVALID)
        {
            puts("[FAT] Could not traverse path (file does not exist).\n");
            return file;
        }
            
    }

    for (int i = 0; i < 10; i++)
    {
        free(parts[i]);
    }

    return file;
}