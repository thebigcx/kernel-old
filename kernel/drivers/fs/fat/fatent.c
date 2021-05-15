#include <drivers/fs/fat/fat.h>

uint64_t fat_cluster_to_lba(fat_dri_t* dri, uint32_t cluster)
{
    return dri->mnt_inf.first_usable_cluster + cluster * dri->mnt_inf.sectors_per_cluster - (2 * dri->mnt_inf.sectors_per_cluster);
}

void* fat_read_cluster_chain(fat_dri_t* dri, uint32_t cluster, uint64_t* num_clus)
{
    uint32_t* clusters = fat_get_cluster_chain(dri, cluster, num_clus);

    uint8_t* buf = page_request();
    uint8_t* _buf = buf;

    for (uint32_t i = 0; i < *num_clus; i++)
    {
        char buffer[100];
        dri->dev->read(dri->dev, fat_cluster_to_lba(dri, clusters[i]), 1, buf);

        buf += 512;
    }

    return _buf;
}

uint32_t* fat_get_cluster_chain(fat_dri_t* dri, uint32_t first_cluster, uint64_t* num_clus)
{
    uint32_t clus = first_cluster;
    uint32_t cchain = 0;
    uint32_t* ret = malloc(100 * sizeof(uint32_t));

    uint8_t* buf = page_request();

    do
    {
        uint32_t fat_sector = dri->mnt_inf.res_sectors + (clus * 4) / 512;
        uint32_t fat_off = (clus * 4) % 512;

        dri->dev->read(dri->dev, fat_sector, 1, buf);
        
        cchain = *((uint32_t*)&buf[fat_off]) & 0x0fffffff;

        ret[(*num_clus)++] = clus;
        clus = cchain;
    }
    while ((cchain != 0) && !((cchain & 0x0fffffff) >= 0x0ffffff8));

    page_free(buf);

    return ret;
}