#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <drivers/storage/dev.h>
#include <drivers/fs/vfs/vfs.h>
#include <stddef.h>

#define FAT_FSINFO_LEAD_SIG     0x41615252
#define FAT_FSINFO_SIG          0x61417272
#define FAT_FSINFO_TRAIL_SIG    0xAA550000

#define FAT_FILE      0
#define FAT_DIRECTORY 1
#define FAT_INVALID   2

#define FAT_ATTR_RO         0x01
#define FAT_ATTR_HIDDEN     0x02
#define FAT_ATTR_SYSTEM     0x04
#define FAT_ATTR_VOL_ID     0x08
#define FAT_ATTR_DIR        0x10
#define FAT_ATTR_ARCHIVE    0x20
#define FAT_ATTR_LFN        0x0f

// BIOS parameter block
typedef struct fat_bpb
{
    uint8_t jmp[3];                 // Jump over format information
    int8_t oem_id[8];               // OEM identifier
    uint16_t sector_sz;             // Bytes per sector
    uint8_t sectors_per_cluster;    // Sectors per cluster
    uint16_t res_sectors;           // Number of reserved sectors
    uint8_t fats;                   // Number of File Allocation Tables
    uint16_t dir_entries;           // Number of directory entries
    uint16_t sector_cnt;            // Total number of sectors in volume
    uint8_t media_desc_type;        // Media descriptor type
    uint16_t sectors_per_fat;       // Number of sectors per FAT (FAT12/FAT16 only)
    uint16_t sectors_per_track;     // Number of sectors per track
    uint16_t head_cnt;              // Number of heads on media
    uint32_t hidden_sectors;        // Number of hidden sectors
    uint32_t large_sector_cnt;      // Large sector count

} __attribute__((packed)) fat_bpb_t;

// FAT32 extended boot record
typedef struct fat_ebr
{
    uint32_t sectors_per_fat;       // Size of FAT in sectors
    uint16_t flags;                 // Flags
    uint16_t fat_vs;                // FAT version
    uint32_t cluster_num;           // Cluster number of root directory
    uint16_t fsinfo_sector;         // The sector number of the FSInfo structure
    uint16_t backup_bs;             // Backup boot sector
    uint8_t res0[12];               // Reserved
    uint8_t drive_num;              // Drive number
    uint8_t res1;                   // Reserved
    uint8_t sig;                    // Signature
    uint32_t volume_id;             // Volume ID serial number
    int8_t volume_label[11];        // Volume label string
    int8_t sys_id[8];               // System identifier string (always "FAT32 ")

} __attribute__((packed)) fat_ebr_t;

// FAT32 FSInfo
typedef struct fat_fsinfo
{
    uint32_t lead_sig;              // Lead signature
    uint8_t res0[480];              // Reserved
    uint64_t sig;                   // Signature
    uint32_t free_clusters;         // Free cluster count
    uint32_t free_cluster_num;      // Cluster number of available clusters
    uint8_t res1[12];               // Reserved
    uint16_t trail_sig;             // Trail signature

} __attribute__((packed)) fat_fsinfo_t;

typedef struct fat_bootrecord
{
    fat_bpb_t bpb;
    fat_ebr_t ebr;

} __attribute__((packed)) fat_bootrecord_t;

typedef struct fat_dir_entry
{
    uint8_t name[8];
    uint8_t ext[3];
    uint8_t attr;
    uint8_t user_attr;

    int8_t undel;
    uint16_t create_time;
    uint16_t create_date;
    uint16_t access_date;
    uint16_t cluster_u;

    uint16_t mod_time;
    uint16_t mod_date;
    uint16_t cluster;
    uint32_t file_sz;

} __attribute__((packed)) fat_dir_entry_t;

typedef struct fat_vol_inf
{
    uint32_t sect_cnt;
    uint32_t fat_off;
    uint32_t root_ent_cnt;
    uint32_t root_offset;
    uint32_t root_sz;
    uint32_t fat_size;
    uint32_t fat_ent_sz;
    uint32_t sects_per_clus;
    uint32_t first_clus;
    uint32_t res_sects;

} fat_vol_inf_t;

// FAT32 filesystem volume
typedef struct fat_vol
{
    dev_t* dev;
    fat_vol_inf_t mnt_inf;

} fat_vol_t;

typedef struct fat_node
{
    char name[32];
    uint32_t flags;
    uint32_t file_len;
    uint32_t cluster;
    fat_vol_t* dri;

} fat_node_t;

typedef struct fat_lfn_entry
{
    uint8_t order;
    uint16_t chars0[5];
    uint8_t attr;
    uint8_t type;
    uint8_t checksum;
    uint16_t chars1[6];
    uint16_t zero;
    uint16_t chars2[2];

} __attribute__((packed)) fat_lfn_entry_t;

// fat.c
bool fat_is_fat(dev_t* dev);
void fat_init(fat_vol_t* dri, dev_t* dev);

int fat_fopen(fs_node_t* file);
void fat_fclose(fs_node_t* file);
size_t fat_fread(fs_node_t* file, void* ptr, size_t off, size_t size);
size_t fat_fwrite(fs_node_t* file, const void* ptr, size_t off, size_t size);
fs_node_t fat_find_file(fs_vol_t* dri, fs_node_t* dir, const char* name);

// file.c
fat_node_t fat_get_file(fat_vol_t* dri, fat_node_t* dir, const char* name);
void fat_file_read(fat_node_t* file, size_t size, size_t off, void* buffer);
void fat_file_write(fat_node_t* file, size_t size, size_t off, void* buffer);
void fat_write_cluster(fat_vol_t* dri, void* buf, uint32_t size, uint32_t cluster);

// dir.c
void fat_read_dir(fat_node_t* node, fat_node_t* files, uint32_t* cnt);

// fatent.c
uint64_t fat_cluster_to_lba(fat_vol_t* dri, uint32_t cluster);
uint32_t* fat_get_cluster_chain(fat_vol_t* dri, uint32_t first_cluster, uint64_t* num_clus);
void* fat_read_cluster_chain(fat_vol_t* dri, uint32_t cluster, uint64_t* num_clus);
void fat_get_lfn(fat_vol_t* dri, char* dst, fat_lfn_entry_t** entries, uint32_t cnt);
void fat_alloc_clusters(fat_vol_t* dri, int* clusters, int num_clusters);