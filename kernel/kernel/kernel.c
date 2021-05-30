#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <drivers/video/video.h>
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

#define LOG(m) console_write(m, 255, 255, 255)
#define DONE() console_write("Done\n", 0, 255, 0)

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

size_t test_read(fs_file_t* file, void* ptr, size_t size)
{
    strcpy((char*)ptr, "Never gonna give you up");
    return 0;
}

size_t test_write(fs_file_t* file, const void* ptr, size_t size)
{
    return 0;
}

void kernel_proc()
{
    DONE();

    // TESTS
    
    fs_node_t dev = vfs_resolve_path("/dev/keyboard", NULL);
    fs_file_t* devfile = vfs_open(&dev);
    char buf[100];

    fs_node_t node = vfs_resolve_path("/system_folder/long_file_name.txt", "/");
    fs_file_t* file = vfs_open(&node);
    uint8_t buffer[100];
    vfs_read(file, buffer, 100);

    for (;;)
    {
        mouse_packet_t pack;
        while (mouse_get_packet(&pack))
        {
            console_write("Mouse\n", 255, 255, 255);
        }

        if (vfs_read(devfile, &buf, 100))
        {
            char ibuffer[100];
            LOG(itoa(*buf, ibuffer, 10));
        }
    }
}

void _start(boot_info_t* inf)
{
    vid_mode_t vidmode;
    vidmode.width = inf->pix_per_line;
    vidmode.height = inf->v_res;
    vidmode.depth = 32;
    vidmode.fb = inf->fb_adr;
    video_init(vidmode);
    video_set_fnt(inf->font);
    memset(vidmode.fb, 0, vidmode.width * vidmode.height * (vidmode.depth / 8));

    LOG("Initializing GDT...");
    gdt_desc_t gdt_desc;
    gdt_desc.size = sizeof(gdt_t) - 1;
    gdt_desc.offset = (uint64_t)&def_gdt;
    gdt_load(&gdt_desc);
    DONE();

    LOG("Initializing paging and memory...");
    init_paging(inf);
    DONE();
    LOG("Initializing heap...");
    heap_init();
    DONE();

    LOG("Initializing keyboard...");
    kb_init();
    DONE();
    LOG("Initializing PIT...");
    pit_init(1600);
    DONE();

    LOG("Initializing mouse...");
    mouse_map_int();
    DONE();
    LOG("Loading IDT...");
    idt_load();
    DONE();

    mouse_init();

    LOG("Enumerating PCI devices...");
    pci_enumerate();
    DONE();
    
    LOG("Initializing AHCI controllers...");
    ahci_init(&pci_devices);
    DONE();

    LOG("Mounting root directory...");

    dev_t dev = ahci_get_dev(0);
    root_mnt_pt = fs_mnt_dev(&dev, "/");

    DONE();

    outb(PIC1_DATA, 0xf8);
    outb(PIC2_DATA, 0xef);
    
    LOG("Creating kernel process...");
    proc_t* proc = mk_proc(kernel_proc);
    sched_spawn_proc(proc);
    DONE();
    
    LOG("Jumping to multitasking...");
    sched_init();

    for (;;);
}