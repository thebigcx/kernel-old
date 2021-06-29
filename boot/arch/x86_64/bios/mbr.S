.code16

#define fat_bpb                     0x7c00
#define fat_reserved_sector_count   0x0e
#define fat_table_count             0x10
#define fat_sectors_per_table       0x16
#define fat_root_entry_count        0x11
#define fat_sectors_per_cluster     0x0d

#define fat_dirent_cluster          0x1a

#define disk_buffer                 0x7e00
#define loader_base                 0x8000

# Skip over the FAT Bios Parameter Block
entry:
    jmp stage1
    nop

.skip 0x3b

stage1:
    xor %ax, %ax
    mov %ax, %ds
    mov $0x7c00, %sp
    
    lea loading_str, %si
    call bios_puts
    call bios_putnl

    call find_loader

    #lea found_loader_msg, %si
    #call bios_puts
    #call bios_putnl
    
    mov fat_dirent_cluster(%bx), %ax
    mov $loader_base, %bx

next_cluster:
    call read_cluster
    cmp $0xfff8, %ax
    jb next_cluster

    #lea found_loader_msg, %si
    #call bios_puts
    #call bios_putnl

    mov $loader_base, %bx
    mov (%bx), %ax

1:
    jmp 1b

# Find loader.bin
# OUT: bx = address of the directory entry
find_loader:
    xor %eax, %eax

# To find root dir:
#  - Add size of the FATs
#  - Add reserved sector count
#  - Multiply root entry count by 32 and divide by 512 (right shift 4)
    mov $fat_bpb, %bx
    mov fat_table_count(%bx), %al
    mul fat_sectors_per_table(%bx)
    add fat_reserved_sector_count(%bx), %ax
    mov fat_root_entry_count(%bx), %dx
    shr $4, %dx
    add $1, %ax

    mov $disk_buffer, %bx
    call bios_read_sector

next_dirent:
    mov %bx, %si
    lea loader_filename, %di
    mov $0x0b, %cx
    repe cmpsb # Compare the strings
    jz found_loader

# Check next directory entry (32 byte size)
    add $0x20, %si
    add $0x20, %bx
    jmp next_dirent

found_loader:
    ret

# Read a file by following the cluster chain
# IN: ax = cluster number
#     bx = destination buffer
read_cluster:
    push %ax
    push %bx

    mov $fat_bpb, %bx

    sub $2, %ax
    mov %ax, %cx

    xor %eax, %eax
    mov fat_table_count(%bx), %al
    mul fat_sectors_per_table(%bx)
    add fat_reserved_sector_count(%bx), %ax
    add %cx, %ax
    mov fat_root_entry_count(%bx), %cx
    shr $4, %cx
    add %cx, %ax

    pop %bx
    call bios_read_sector
    add $512, %bx
    
    pop %ax
    push %bx

    # Use FAT to find next sector in cluster chain
    mov $fat_bpb, %bx
    shl $1, %ax
    mov $512, %cx
    #divw %cx # Remainder of division stored in dx, saves an extra modulo operation
    # TEMP
    mov %ax, %dx
    mov $0, %ax
    
    add fat_reserved_sector_count(%bx), %ax

    mov $disk_buffer, %bx
    call bios_read_sector

    add %dx, %bx
    mov (%bx), %ax
    pop %bx

    ret

# BIOS read sector
# IN: ax = LBA
#     es:bx = buffer
bios_read_sector:
    mov %al, %cl
    push %ax

    mov $0x02, %ah
    mov $0x01, %al
    mov $0x00, %ch
    mov $0x00, %dh
    mov $0x80, %dl

    int $0x13

    pop %ax
    ret

# Print a string to screen
# IN: si = pointer to null-terminated string
bios_puts:
    push %ax
    mov $0x0e, %ah

bios_puts_loop:
    mov (%si), %al
    add $1, %si
    cmp $0, %al
    je bios_puts_end
    int $0x10
    
    jmp bios_puts_loop
bios_puts_end:
    pop %ax
    ret

bios_putnl:
    push %ax

    mov $0x0e, %ah
    mov $0x0a, %al
    int $0x10
    mov $0x0d, %al
    int $0x10

    pop %ax
    ret

loading_str: .string "Loading. Please wait."
found_loader_msg: .string "Found loader."
loader_filename: .string "LOADER  BIN"

.skip 510 - (. - entry)
.word 0xaa55
