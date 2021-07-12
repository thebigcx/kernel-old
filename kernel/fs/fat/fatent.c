#include <fs/fat/fat.h>

#include <mem/kheap.h>

uint64_t fat_cluster_to_lba(fat_vol_t* vol, uint32_t cluster)
{
    return vol->mnt_inf.first_clus + cluster * vol->mnt_inf.sects_per_clus - (2 * vol->mnt_inf.sects_per_clus);
}

void* fat_read_cluster_chain(fat_vol_t* vol, uint32_t cluster, uint64_t* num_clus)
{
    uint32_t* clusters = fat_get_cluster_chain(vol, cluster, num_clus);

    uint8_t* buf = kmalloc(*num_clus * 512);
    uint8_t* _buf = buf;

    for (uint32_t i = 0; i < *num_clus; i++)
    {
        /*if (!vol->dev->read(vol->dev, buf, fat_cluster_to_lba(vol, clusters[i]), 1))
        {
            console_write("[FAT32] Device failed to read sector\n", 255, 0, 0);
            kfree(clusters);
            return _buf;
        }*/
        vol->dev->read(vol->dev, buf, fat_cluster_to_lba(vol, clusters[i]), 1);

        buf += 512;
    }

    kfree(clusters);
    
    return _buf;
}

uint32_t* fat_get_cluster_chain(fat_vol_t* vol, uint32_t first_cluster, uint64_t* num_clus)
{
    uint32_t clus = first_cluster;
    uint32_t cchain = 0;
    uint32_t* ret = kmalloc(1000 * sizeof(uint32_t));

    uint8_t* buf = kmalloc(512);

    do
    {
        uint32_t fat_sector = vol->mnt_inf.res_sects + (clus * 4) / 512;
        uint32_t fat_off = (clus * 4) % 512;

        /*if (!vol->dev->read(vol->dev, buf, fat_sector, 1))
        {
            console_write("[FAT32] Device failed to read sector\n", 255, 0, 0);
            kfree(buf);
            return ret;
        }*/
        vol->dev->read(vol->dev, buf, fat_sector, 1);
        
        cchain = *((uint32_t*)&buf[fat_off]) & 0x0fffffff;

        ret[(*num_clus)++] = clus;
        clus = cchain;
    }
    while ((cchain != 0) && !((cchain & 0x0fffffff) >= 0x0ffffff8));

    kfree(buf);

    return ret;
}

void fat_get_lfn(fat_vol_t* vol, char* dst, fat_lfn_entry_t** entries, uint32_t cnt)
{
    uint32_t name_idx = 0;

    for (int32_t i = cnt - 1; i >= 0; i--)
    {
        for (uint32_t j = 0; j < 5; j++)
            dst[name_idx++] = entries[i]->chars0[j];

        for (uint32_t j = 0; j < 6; j++)
            dst[name_idx++] = entries[i]->chars1[j];

        dst[name_idx++] = entries[i]->chars2[0];
        dst[name_idx++] = entries[i]->chars2[1];
    }

    dst[name_idx] = '\0';
}

uint32_t fat_find_free_clus(fat_vol_t* vol)
{
    uint32_t cchain = 0;

}

void fat_alloc_clusters(fat_vol_t* vol, int* clusters, int num_clusters)
{
    
}