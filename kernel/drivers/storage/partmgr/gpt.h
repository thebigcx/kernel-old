#pragma once

#include <util/types.h>
#include <drivers/fs/vfs/vfs.h>

#define GPT_ATTR_RES    (1     ) // Reserved for firmware
#define GPT_ATTR_BOOT   (1 << 1) // Necessary for os boot

typedef struct gpt_hdr
{
    uint8_t sig[8];
    uint32_t revision;
    uint32_t hdrsize;
    uint32_t checksum;
    uint32_t res;
    uint64_t hdrlba;
    uint64_t altlba; // LBA containing alternate GPT header
    uint64_t first_datablk;
    uint64_t last_datablk;
    uint8_t guid[16];
    uint64_t entlba;
    uint32_t nrentries;
    uint32_t entsize;
    uint32_t arrchecksum;

} __attribute__((packed)) gpt_hdr_t;

typedef struct gpt_part_ent
{
    uint8_t type_guid[16];
    uint8_t unique_guid[16];
    uint64_t lba_start;
    uint64_t lba_end;
    uint64_t attr;
    uint16_t name[36]; // Unicode 16-bit

} __attribute__((packed)) gpt_part_ent_t;

// GPT partition device
typedef struct gpt_partdev
{
    vfs_node_t* disk;
    uint64_t startlba;
    uint64_t endlba;
    uint16_t name[36];   

} gpt_partdev_t;

vfs_node_t* gpt_getpart(vfs_node_t* disk, uint16_t* name);

size_t gpt_partread(vfs_node_t* node, void* ptr, size_t off, size_t size);
size_t gpt_partwrite(vfs_node_t* node, const void* ptr, size_t off, size_t size);