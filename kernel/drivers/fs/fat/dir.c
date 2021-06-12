#include <drivers/fs/fat/fat.h>
#include <string.h>
#include <stdlib.h>
#include <mem/heap.h>

void fat_read_dir(fat_node_t* node, fat_node_t* files, uint32_t* cnt)
{
    uint64_t numclus = 0;

    fat_dir_entry_t* dirs = fat_read_cluster_chain(node->dri, node->cluster, &numclus);

    fat_lfn_entry_t** lfn_entries = kmalloc(sizeof(fat_lfn_entry_t*) * 10);
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
            if (lfn_cnt)
            {
                fat_get_lfn(node->dri, files[*cnt].name, lfn_entries, lfn_cnt);
            }
            else
            {
                strcpy(files[*cnt].name, (char*)dirs[i].name);
                strcpy(files[*cnt].name + 8, (char*)dirs[i].ext);
                files[*cnt].name[11] = '\0';
            }

            files[*cnt].cluster = (((uint32_t)dirs[i].cluster_u) << 16) | dirs[i].cluster;
            files[*cnt].file_len = dirs[i].file_sz;

            files[*cnt].flags = dirs[i].attr & FAT_ATTR_DIR ? FAT_DIRECTORY : FAT_FILE;
            files[*cnt].dri = node->dri;
            
            (*cnt)++;
        }
    }

    kfree(lfn_entries);
}