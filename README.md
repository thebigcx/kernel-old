# Operating System
A hobby operating system for x86 64-bit computers, based on the Stivale2 specification.

## Features
- Paging and Virtual Memory
- Higher half kernel
- Dynamic heap memory (`kmalloc` and `kfree`)
- Generic VESA/GOP framebuffer driver
- PS/2 mouse and keyboard input
- Reading and writing to SATA disks through AHCI
- VFS, Ext2 and FAT 32 filesystem drivers
- Pre-emptive multitasking with round-robin scheduler
- Syscalls and User-space

## Coming Soon
- UHCI driver for USB
- Symmetric Multiprocessing (SMP)
- Window Manager
