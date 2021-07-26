# KERNEL

The kernel for my operating system.

## Features
- Paging/Virtual memory
- Higher half kernel
- VESA framebuffer driver
- PS/2 mouse and keyboard drivers
- Serial ATA disk r/w through AHCI driver
- VFS, Ext2, and FAT32 fs drivers
- Pre-emptive multitasking
- SMP with support for up to 64 cores
- Syscalls and User-space

## Building
GNU Make, binutils, and GCC are required.

Run ```make``` to generate an ELF file called kernel.elf, which can be loaded by a Stivale2-compliant bootloader, such as [limine](https://github.com/limine-bootloader/limine).
