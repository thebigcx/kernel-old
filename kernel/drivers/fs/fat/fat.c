#include <drivers/fs/fat/fat.h>
#include <paging/paging.h>


#include <mem/heap.h>

bool fat_is_fat(dev_t* dev)
{
    fat_bootrecord_t* record = kmalloc(512);

    dev->read(dev, 0, 1, record);

    if (record->ebr.sig == 0x28 || record->ebr.sig == 0x29)
    {
        uint32_t data_sectors = record->bpb.large_sector_cnt - (record->bpb.res_sectors + (record->ebr.sectors_per_fat * record->bpb.fats));
        uint32_t clusters = data_sectors / record->bpb.sectors_per_cluster;

        if (clusters > 65525) return true;
    }

    kfree(record);

    return false;
}

void fat_init(fat_vol_t* vol, dev_t* dev)
{
    fat_bootrecord_t* record = kmalloc(512);
    dev->read(dev, 0, 1, record);

    vol->mnt_inf.sect_cnt = record->bpb.sector_cnt;
    vol->mnt_inf.fat_off = 1;
    vol->mnt_inf.fat_size = record->ebr.sectors_per_fat;
    vol->mnt_inf.fat_ent_sz = 8;
    vol->mnt_inf.root_ent_cnt = record->bpb.dir_entries;
    vol->mnt_inf.root_offset = record->ebr.cluster_num;
    vol->mnt_inf.root_sz = (record->bpb.dir_entries * 32) / record->bpb.sector_sz;
    vol->mnt_inf.sects_per_clus = record->bpb.sectors_per_cluster;
    vol->mnt_inf.res_sects = record->bpb.res_sectors;
    vol->mnt_inf.first_clus = record->bpb.res_sectors + record->bpb.fats * record->ebr.sectors_per_fat;

    vol->dev = dev;

    kfree(record);
}

size_t fat_fread(fs_node_t* file, void* ptr, size_t off, size_t size)
{
    return fat_file_read((fat_node_t*)file->derived, size, off, ptr);
}

size_t fat_fwrite(fs_node_t* file, const void* ptr, size_t off, size_t size)
{

}

int fat_fopen(fs_node_t* file)
{
    return file->derived == NULL;
}

void fat_fclose(fs_node_t* file)
{

}

fs_node_t fat_find_file(fs_vol_t* vol, fs_node_t* dir, const char* name)
{
    fat_node_t* fatfile = kmalloc(sizeof(fat_node_t));
    fat_node_t* parent = dir ? (fat_node_t*)dir->derived : NULL;
    
    *fatfile = fat_get_file((fat_vol_t*)(vol->derived), parent, name);

    fs_node_t node;
    node.derived = fatfile;
    node.read = fat_fread;
    node.write = fat_fwrite;
    node.open = fat_fopen;
    node.close = fat_fclose;

    return node;
}