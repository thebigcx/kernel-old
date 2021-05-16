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
#include <pit.h>
#include <acpi.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_ids.h>
#include <drivers/storage/ahci.h>
#include <drivers/fs/fat/fat.h>

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
        page_map_memory((void*)i, (void*)i);
    }

    uint64_t fb_size = inf->fb_buf_sz + PAGE_SIZE;
    page_alloc_m((void*)inf->fb_adr, fb_size / PAGE_SIZE + 1);
    for (uint64_t i = inf->fb_adr; i < inf->fb_adr + fb_size; i += PAGE_SIZE)
    {
        page_map_memory((void*)i, (void*)i);
    }

    asm ("mov %0, %%cr3"::"r"(page_get_pml4()));
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
    heap_init((void*)0x0000100000000000, 16);

    keyboard_init();
    mouse_init();
    pit_init();

    idt_load();

    mouse_start();

    acpi_mcfg_hdr_t* hdr = acpi_find_facp((acpi_sdt_hdr_t*)(inf->rsdp->xsdt_addr));
    pci_enumerate(hdr);
    for (uint32_t i = 0; i < pci_devices.count; i++)
    {
        pci_dev_t* dev = &pci_devices.devs[i];
        
        if (dev->class_code == PCI_CLASS_STORAGE && dev->subclass == PCI_SUBCLASS_SATA && dev->progif == PCI_PROGIF_AHCI)
        {
            ahci_init(dev);
        }
    }
    storage_dev_t dev = ahci_get_dev(0);
    bool t = fat_is_fat(&dev);
    fat_dri_t fat_dri;
    fat_init(&fat_dri, &dev);

    fat_file_t* files = page_request();
    uint32_t cnt = 0;
    //fat_file_t system = fat_get_file(&fat_dri, NULL, "SYSTEM     ");
    fat_file_t file = fat_get_file(&fat_dri, NULL, "long_file_name.txt");
    //fat_read_dir(&fat_dri, fat_dri.mnt_inf.root_offset, files, &cnt);
    {
        puts(file.name);
        puts("\n");
        puts("======Contents=======\n\n");

        void* buffer = page_request();
        fat_file_read(&fat_dri, &file, file.file_len, buffer);

        for (int i = 0; i < file.file_len; i++)
        {
            putchar(((char*)buffer)[i]);
        }

        puts("\n=======EOF=========\n");
    }
    //fat_read_dir(&fat_dri, file.curr_cluster, files, &cnt);
    //fat_read_dir(&fat_dri, 2, files, &cnt);
    /*fat_file_t sys = fat_get_file(&fat_dri, NULL, "SYSTEM     ");
    fat_read_dir(&fat_dri, sys.curr_cluster, files, &cnt);

    for (int i = 0; i < cnt; i++)
    {
        puts(files[i].name);
        puts("\n");
        puts("======Contents=======\n\n");

        void* buffer = page_request();
        fat_file_read(&fat_dri, &files[i], files[i].file_len, buffer);

        for (int i = 0; i < files[i].file_len; i++)
        {
            putchar(((char*)buffer)[i]);
        }

        puts("\n=======EOF=========\n");
    }*/

    //while (1);

    outb(PIC1_DATA, 0xfd);
    outb(PIC2_DATA, 0xff);

    asm ("sti");

    //pit_set_count(2000);
    
    //gr_clear();
    gl_surface_clear(&fb_surf, 0, 0, 0);
    /*for (int x = 0; x < 100; x++)
    for (int y = 0; y < 100; y++)
    {
        *((uint32_t*)(graphics_data.fb_adr + 4 * graphics_data.pix_per_line * x + 4 * y)) = *color;
    }*/

    char buffer[100];

    while (1)
    {
        uint32_t key;
        while (keyboard_get_key(&key))
        {
            //char buffer[10];
            //sprintf(buffer, "%d\n", key);
            //sprintf(buffer, "%s\n", "Key pressed!");
            //printf(buffer);
            printf("Key pressed: %d\n", key);
        }
            //puts(itoa(key, buffer, 16));

        //printf("Hello!", 5);

        //gl_surface_clear(&fb_surf, 0, 0, 0);
    }

    return;
}