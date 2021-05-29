#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <drivers/graphics/graphics.h>
#include <paging/paging.h>
#include <mem/mem.h>
#include <mem/heap.h>
#include <gdt/idt.h>
#include <gdt/gdt.h>
#include <drivers/input/mouse/ps2_mouse.h>
#include <drivers/input/keyboard/ps2_keyboard.h>
#include <io.h>
#include <time/time.h>
#include <acpi.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_ids.h>
#include <drivers/storage/ahci.h>
#include <drivers/fs/vfs/vfs.h>
#include <sched/sched.h>

extern void jump_usermode();

// Defined in linker
extern uint64_t _KernelStart;
extern uint64_t _KernelEnd;

typedef struct
{
    uint64_t mem_map_size;
    uint64_t mem_map_key;
    uint64_t mem_map_desc_size;
    uint32_t mem_map_desc_vers;
    void* mem_map;

    uint64_t fb_adr;
    uint32_t pix_per_line;
    uint32_t v_res;
    uint64_t fb_buf_sz;

    psf1_font* font;

    acpi_rsdt2_t* rsdp;
} boot_info_t;

static void init_paging(boot_info_t* inf)
{
    paging_init(inf->mem_map, inf->mem_map_size, inf->mem_map_desc_size);

    // Reserve memory for kernel
    uint64_t kernel_sz = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    void* kernel_start = (void*)&_KernelStart;
    uint64_t kernel_pg_cnt = kernel_sz / PAGE_SIZE + 1;

    page_alloc_m(kernel_start, kernel_pg_cnt);

    uint64_t mem_size = get_memory_size(inf->mem_map, inf->mem_map_size / inf->mem_map_desc_size, inf->mem_map_desc_size);
    for (uint64_t i = 0; i < mem_size; i += PAGE_SIZE)
    {
        page_kernel_map_memory((void*)i, (void*)i);
    }

    uint64_t fb_size = inf->fb_buf_sz + PAGE_SIZE;
    page_alloc_m((void*)inf->fb_adr, fb_size / PAGE_SIZE + 1);
    for (uint64_t i = inf->fb_adr; i < inf->fb_adr + fb_size; i += PAGE_SIZE)
    {
        page_kernel_map_memory((void*)i, (void*)i);
    }

    asm ("mov %0, %%cr3"::"r"(page_get_kpml4()));
}

void kernel_proc()
{
    puts("KERNEL PROCESS");
    for (;;)
    {
        mouse_packet_t pack;
        while (mouse_get_packet(&pack))
        {
            puts("Mouse\n");
        }
    }
}

void _start(boot_info_t* inf)
{
    gl_surface_t fb_surf;
    fb_surf.width = inf->pix_per_line;
    fb_surf.height = inf->v_res;
    fb_surf.buffer = inf->fb_adr;

    graphics_data.font = inf->font;
    graphics_data.fb_surf = fb_surf;

    gdt_desc_t gdt_desc;
    gdt_desc.size = sizeof(gdt_t) - 1;
    gdt_desc.offset = (uint64_t)&def_gdt;
    gdt_load(&gdt_desc);

    init_paging(inf);
    heap_init((void*)0x0000100000000000);

    kb_init();
    pit_init(1600);

    mouse_map_int();
    idt_load();

    mouse_init();

    pci_enumerate();
    
    ahci_init(&pci_devices);
    dev_t dev = ahci_get_dev(0);

    fs_mnt_disk(&dev, &root_mnt_pt);
    fs_file_t* file = vfs_open("system_folder/long_file_name.txt");

    uint8_t buf[100];
    vfs_read(buf, 100, file);

    for (int i = 0; i < 100; i++)
    {
        putchar(buf[i]);
    }

    vfs_read(buf, 100, file);

    for (int i = 0; i < 100; i++)
    {
        putchar(buf[i]);
    }

    outb(PIC1_DATA, 0xf8);
    outb(PIC2_DATA, 0xef);

    asm ("sti");
    
    sched_init();

    for (;;);
}