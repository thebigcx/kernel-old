#include <drivers/fs/fat/fat.h>
#include <paging/paging.h>
#include <stdlib.h>
#include <stdio.h>

bool fat_is_fat(dev_t* dev)
{
    fat_bootrecord_t* record = page_request();

    dev->read(dev, 0, 1, record);

    if (record->ebr.sig == 0x28 || record->ebr.sig == 0x29)
    {
        uint32_t data_sectors = record->bpb.large_sector_cnt - (record->bpb.res_sectors + (record->ebr.sectors_per_fat * record->bpb.fats));
        uint32_t clusters = data_sectors / record->bpb.sectors_per_cluster;

        if (clusters > 65525) return true;
    }

    page_free(record);

    return false;
}

void fat_init(fat_dri_t* dri, dev_t* dev)
{
    fat_bootrecord_t* record = page_request();
    dev->read(dev, 0, 1, record);

    char buffer[100];
    puts("[FAT32] Initializing Volume : Signature: ");
    puts(itoa(record->ebr.sig, buffer, 10));
    puts(", OEM ID: ");
    puts(record->bpb.oem_id);
    puts(", Size: ");
    puts(itoa(record->bpb.sector_cnt * 512 / 1024 / 1024, buffer, 10));
    puts(" MB\n");

    dri->mnt_inf.sector_cnt = record->bpb.sector_cnt;
    dri->mnt_inf.fat_offset = 1;
    dri->mnt_inf.fat_size = record->ebr.sectors_per_fat;
    dri->mnt_inf.fat_entry_sz = 8;
    dri->mnt_inf.root_entry_cnt = record->bpb.dir_entries;
    dri->mnt_inf.root_offset = record->ebr.cluster_num;
    dri->mnt_inf.root_sz = (record->bpb.dir_entries * 32) / record->bpb.sector_sz;
    dri->mnt_inf.sectors_per_cluster = record->bpb.sectors_per_cluster;
    dri->mnt_inf.res_sectors = record->bpb.res_sectors;
    dri->mnt_inf.first_usable_cluster = record->bpb.res_sectors + record->bpb.fats * record->ebr.sectors_per_fat;

    dri->dev = dev;

    page_free(record);
}

fs_file_t* fat_fopen(fs_dri_t* dri, const char* path)
{
    fs_file_t* file = malloc(sizeof(fs_file_t));
    file->id = 0;
    file->priv = malloc(sizeof(fat_file_t));
    file->pos = 0;
    
    *((fat_file_t*)file->priv) = fat_traverse_path((fat_dri_t*)dri->priv, path);

    return file;
}

size_t fat_fread(fs_dri_t* dri, void* ptr, size_t size, fs_file_t* stream)
{
    fat_file_read((fat_dri_t*)dri->priv, (fat_file_t*)stream->priv, size, stream->pos, ptr);
    stream->pos += size;
}

size_t fat_fwrite(fs_dri_t* dri, const void* ptr, size_t size, fs_file_t* stream)
{

}

int fat_fclose(fs_dri_t* dri, fs_file_t* stream)
{

}