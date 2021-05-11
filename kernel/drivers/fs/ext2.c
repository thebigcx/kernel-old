#include <drivers/fs/ext2.h>
#include <stdint.h>

#define EXT2_FS_STATE_CLEAN 1
#define EXT2_FS_STATE_ERR   2

#define EXT2_ERR_ACT_CONTINUE 1
#define EXT2_ERR_ACT_REMOUNT  2
#define EXT2_ERR_ACT_PANIC    3

#define EXT2_CREATOR_OS_LINUX   0
#define EXT2_CREATOR_OS_HURD    1
#define EXT2_CREATOR_OS_MASIX   2
#define EXT2_CREATOR_OS_FREEBSD 3
#define EXT2_CREATOR_OS_LITES   4

#define EXT2_OPTFLAG_PREALLOC   0x1  // Prealloc directory blocks
#define EXT2_OPTFLAG_AFS_INODES 0x2  // AFS server inodes exist
#define EXT2_OPTFLAG_JOURNAL    0x4  // Has a journal
#define EXT2_OPTFLAG_EXT_ATTR   0x8  // Inodes have extended attributes
#define EXT2_OPTFLAG_RESIZE     0x10 // Can resize itself for large partitions
#define EXT2_OPTFLAG_HASH_IDX   0x20 // Directories use hash index

#define EXT2_REQFLAG_COMPRESSION 0x1 // Compression is used
#define EXT2_REQFLAG_FILETYPE    0x2 // Directory entries contain type field
#define EXT2_REQFLAG_RECOVER     0x4 // Needs to recover
#define EXT2_REQFLAG_JOURNAL     0x8 // Uses journal device

#define EXT2_ROFLAG_SPARSE  0x1 // Sparse superblocks and group descriptor tables
#define EXT2_ROFLAG_64BIT   0x2 // Uses 64-bit file size
#define EXT2_ROFLAG_BINTREE 0x4 // Directory contents stored as binary tree

typedef struct ext2_superblock
{
    uint32_t total_inodes;      // Total number of inodes in filesystem
    uint32_t total_blocks;      // Total number of blocks in filesystem
    uint32_t res_blocks;        // Number of blocks reserved for superuser
    uint32_t free_blocks;       // Number of unallocated blocks
    uint32_t free_inodes;       // Number of free inodes
    uint32_t first_dat_block;   // Block containing this superblock
    uint32_t log_block_sz;      // log2(block_sz) - 10
    uint32_t log_frag_sz;       // log2(frag_sz) - 10
    uint32_t blocks_per_group;  // Number of blocks per block group
    uint32_t frags_per_group;   // Number of fragments per block group
    uint32_t inodes_per_group;  // Number of inodes per block group
    uint32_t mnt_time;          // Last mount time (POSIX time)
    uint32_t write_time;        // Last write time (POSIX time)

    uint16_t mnt_cnt;           // Number of times volume has been mounted
    uint16_t max_mnt_cnt;       // Maximum mounts before consistency check
    uint16_t ext2_sig;          // Ext2 signature
    uint16_t state;             // Filesystem state
    uint16_t errors;            // What to do when error detected
    uint16_t min_version;       // Minor portion of version
    
    uint32_t last_check;        // POSIX time of last check
    uint32_t check_intvl;       // Interval (in POSIX time) between checks
    uint32_t os_id;             // Operating system ID of creation
    uint32_t maj_version;       // Major portion of version

    uint16_t res_uid;           // User ID that can use reserved blocks
    uint16_t res_gid;           // Group ID that can use reserved blocks

} __attribute__((packed)) ext2_superblock_t;

typedef struct ext2_superblock_ext
{
    uint32_t first_inode;           // First non-reserved inode
    uint16_t inode_sz;              // Size of inode structure in bytes
    uint16_t block_group;           // Block group that this superblock is part of
    uint32_t features_compat;       // Optional features present
    uint32_t features_req;          // Required features present
    uint32_t features_ro;           // Read-only features present
    uint8_t fs_id[16];              // Filesystem ID
    char volume_name[16];           // Volume name
    char last_mnt[64];              // Path this volume was last mounted to
    uint32_t alg_bitmap;            // Compression algorithms used
    uint8_t prealloc_blocks;        // Number of blocks to preallocate for files
    uint8_t prealloc_dir_blocks;    // Number of blocks to preallocate for directories
    uint16_t unused;                // Unused
    uint8_t padding[16];            // Padding
    char journal_id[16];            // Journal ID
    uint32_t journal_inode;         // Journal inode
    uint32_t journal_dev;           // Journal device
    uint32_t orpan_inode_head;      // Head of orphan inode list

} __attribute__((packed)) ext2_superblock_ext_t;

typedef struct ext2_group_desc_tbl
{
    uint32_t block_bitmap_addr;     // Block address of block usage bitmap
    uint32_t inode_bitmap_addr;     // Block address of inode usage bitmap
    uint32_t inode_tbl_addr;        // Starting block address of inode table
    uint32_t free_blocks;           // Number of free blocks in group
    uint32_t free_inodes;           // Number of free inodes in group
    uint32_t dir_cnt;               // Number of directories in group

} __attribute__((packed)) ext2_group_desc_tbl_t;