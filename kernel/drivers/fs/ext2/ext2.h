#pragma once

#include <util/types.h>
#include <drivers/fs/vfs/vfs.h>
#include <util/list.h>

#define EXT2_SB_LOC     1024
#define EXT2_SB_LBA     2
#define EXT2_ROOT_DIR   2

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

// Inode types
#define EXT2_INODE_TYPE_FIFO        0x1000 // FIFO
#define EXT2_INODE_TYPE_CHAR_DEV    0x2000 // Character device
#define EXT2_INODE_TYPE_DIR         0x4000 // Directory
#define EXT2_INODE_TYPE_BLOCK_DEV   0x6000 // Block device
#define EXT2_INODE_TYPE_REGFILE     0x8000 // Regular file
#define EXT2_INODE_TYPE_SYMLINK     0xa000 // Symbolic link
#define EXT2_INODE_TYPE_UNIX_SOCKET 0xc000 // Unix socket

// Inode permissions
#define EXT2_INODE_PERM_OTHER_EXEC   0x001 // Other - execute permission
#define EXT2_INODE_PERM_OTHER_WRITE  0x002 // Other - write permission
#define EXT2_INODE_PERM_OTHER_READ   0x004 // Other - read permission
#define EXT2_INODE_PERM_GROUP_EXEC   0x008 // Group - execute permission
#define EXT2_INODE_PERM_GROUP_WRITE  0x010 // Group - write permission
#define EXT2_INODE_PERM_GROUP_READ   0x020 // Group - read permission
#define EXT2_INODE_PERM_USER_EXEC    0x040 // User - execute permission
#define EXT2_INODE_PERM_USER_WRITE   0x080 // User - write permission
#define EXT2_INODE_PERM_USER_READ    0x100 // User - read permission
#define EXT2_INODE_PERM_STICKY       0x200 // Sticky bit
#define EXT2_INODE_PERM_SET_GID      0x400 // Set Group ID
#define EXT2_INODE_PERM_SET_UID      0x800 // Set User ID
 
// Inode flags
#define EXT2_FL_SECRM       0x00000001 // Secure deletion
#define EXT2_FL_UNRM        0x00000002 // Undelete
#define EXT2_FL_COMPRESS    0x00000004 // File compression
#define EXT2_FL_SYNC_UPDATE 0x00000008 // Synchonous updates
#define EXT2_FL_IMMUT       0x00000010 // Immutable file
#define EXT2_FL_APPEND_ONLY 0x00000020 // Append only
#define EXT2_FL_DUMP        0x00000040 // File is not included in 'dump' command
#define EXT2_FL_NOATIME     0x00000080 // Do not update atime
#define EXT2_FL_HASH_DIR    0x00010000 // Hash indexed directory
#define EXT2_FL_AFS_DIR     0x00020000 // AFS directory
#define EXT2_FL_JOURNAL     0x00040000 // Journal file data

// File types
#define EXT2_UNKNOWN        0 // Unknown file
#define EXT2_REGFILE        1 // Regular file
#define EXT2_DIR            2 // Directory
#define EXT2_CHAR_DEV       3 // Character device
#define EXT2_BLOCK_DEV      4 // Block device
#define EXT2_FIFO           5 // FIFO
#define EXT2_SOCKET         6 // Socket
#define EXT2_SYMLINK        7 // Symbolic link

typedef struct ext2_superblock
{
    uint32_t inode_cnt;         // Total number of inodes in filesystem
    uint32_t blk_cnt;           // Total number of blocks in filesystem
    uint32_t res_blocks;        // Number of blocks reserved for superuser
    uint32_t free_blocks;       // Number of unallocated blocks
    uint32_t free_inodes;       // Number of free inodes
    uint32_t first_dat_block;   // Block containing this superblock
    uint32_t log_block_sz;      // log2(block_sz) - 10
    uint32_t log_frag_sz;       // log2(frag_sz) - 10
    uint32_t blks_per_grp;      // Number of blocks per block group
    uint32_t frags_per_grp;     // Number of fragments per block group
    uint32_t inodes_per_grp;    // Number of inodes per block group
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

typedef struct ext2_sb
{
    ext2_superblock_t sb;
    ext2_superblock_ext_t sbext;

} __attribute__((packed)) ext2_sb_t;

typedef struct ext2_group_desc_tbl
{
    uint32_t block_bitmap_addr;     // Block address of block usage bitmap
    uint32_t inode_bitmap_addr;     // Block address of inode usage bitmap
    uint32_t inode_tbl_addr;        // Starting block address of inode table
    uint32_t free_blocks;           // Number of free blocks in group
    uint32_t free_inodes;           // Number of free inodes in group
    uint32_t dir_cnt;               // Number of directories in group
    uint16_t padding;
    uint8_t res[12];

} __attribute__((packed)) ext2_group_desc_tbl_t;

typedef struct ext2_inode
{
    uint16_t mode;          // Types and Permissions
    uint16_t userid;        // User ID
    uint32_t size;          // Lower 32 bits of size
    uint32_t last_access;   // Last access time in POSIX time
    uint32_t creation_time; // Creation time in POSIX time
    uint32_t last_mod_time; // Last modification time in POSIX time
    uint32_t del_time;      // Deletion time in POSIX time
    uint16_t grpid;         // Group ID
    uint16_t link_cnt;      // Amount of hard links (directory entries)
    uint32_t sector_cnt;    // Count of disk sectors
    uint32_t flags;         // Flags
    uint32_t os_spec1;      // OS-specific value #1
    uint32_t blocks[15];    // Direct/Indirect block pointers
    uint32_t gen_num;       // Generation number
    uint32_t file_acl;      // Extended attributes for file
    union
    {
        uint32_t dir_acl;   // Directory attributes
        uint32_t size_u;    // File size upper 32 bits
    };
    uint32_t frag_addr;     // Block address of fragment
    uint8_t os_spec2[12];   // OS-specific value #2

} __attribute__((packed)) ext2_inode_t;

#define EXT2_DIRENT_NAME_OFF 8

typedef struct ext2_dir_ent
{
    uint32_t inode;     // Inode
    uint16_t size;      // Total size of this field
    uint8_t name_len;   // Name length least-significant 8 bits
    uint8_t file_type;  // Type indicator
    // Name here

} __attribute__((packed)) ext2_dir_ent_t;

typedef struct ext2_vol
{
    vfs_node_t* dev;
    uint32_t blk_sz;
    ext2_group_desc_tbl_t* blk_grps;
    uint32_t blk_grp_cnt;

    struct
    {
        ext2_superblock_t super;
        ext2_superblock_ext_t superext;
    } __attribute__((packed));

} ext2_vol_t;

typedef struct ext2_node
{
    uint32_t ino;
    ext2_vol_t* vol;

} ext2_node_t;

// inode.c
void ext2_read_inode(ext2_vol_t* vol, uint32_t num, ext2_inode_t* inode);
void ext2_write_inode(ext2_vol_t* vol, uint32_t num, ext2_inode_t* inode);
uint32_t* ext2_get_inode_blks(ext2_vol_t* vol, uint32_t idx, uint32_t cnt, ext2_inode_t* ino);
uint32_t ext2_get_inode_blk(ext2_vol_t* vol, uint32_t idx, ext2_inode_t* ino);
size_t ext2_get_size(ext2_inode_t* ino);
void ext2_alloc_inode_blk(ext2_vol_t* vol, ext2_inode_t* ino, uint32_t ino_num);

// file.c
fs_fd_t* ext2_open(vfs_node_t* file, uint32_t flags);
size_t ext2_read(vfs_node_t* node, void* ptr, size_t off, size_t size);
size_t ext2_write(vfs_node_t* file, const void* ptr, size_t off, size_t size);
void ext2_close(vfs_node_t* file);
void ext2_mkfile(vfs_node_t* parent, const char* name);
void ext2_mkdir(vfs_node_t* parent, const char* name);

// dir.c
vfs_node_t* ext2_finddir(vfs_node_t* dir, const char* name);
list_t* ext2_listdir(vfs_node_t* parent);

vfs_node_t* ext2_dirent_to_node(ext2_vol_t* vol, ext2_dir_ent_t* dirent);

// super.c
vfs_node_t* ext2_init(vfs_node_t* dev);
uint8_t ext2_is_ext2(vfs_node_t* dev);
uint32_t ext2_inode_bg(ext2_vol_t* vol, uint32_t inode);
uint32_t ext2_inode_bg_idx(ext2_vol_t* vol, uint32_t inode);
uint64_t ext2_inode_lba(ext2_vol_t* vol, uint32_t inode);
uint32_t ext2_loc_to_blk(ext2_vol_t* vol, uint64_t loc);
uint64_t ext2_blk_to_lba(ext2_vol_t* vol, uint64_t blk);
uint32_t ext2_alloc_block(ext2_vol_t* vol);