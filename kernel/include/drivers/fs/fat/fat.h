#pragma once

#include "stdint.h"

// FAT32 boot sector struct
typedef struct
{
    uint8_t magic0, res0, magic1;
    uint8_t oem_name[8];
    uint16_t bytes_per_sector;
    uint8_t sectors_per_cluster;
    uint16_t reserved_sectors;
    uint8_t table_count;
    uint16_t root_entry_count;
    uint16_t sectors;
    uint8_t media_type;
    uint16_t table_size;
    uint16_t sectors_per_track;
    uint16_t head_side_count;
    uint32_t hidden_sector_count;
    uint32_t sectors32;

    uint8_t extended_section[54];

} __attribute__((packed)) fat_bs_32_t;