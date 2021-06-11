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
#include <apic.h>
#include <unistd.h>
#include <syscall.h>

#define LOG(m) console_write(m, 255, 255, 255)
#define DONE() console_write("Done\n", 0, 255, 0)

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

    acpi_rsdp_t* rsdp;
} boot_info_t;

static void init_paging(boot_info_t* inf)
{
    paging_init(inf->mem_map, inf->mem_map_size, inf->mem_map_desc_size);

    // Reserve memory for kernel
    uint64_t kernel_sz = (uint64_t)&_KernelEnd - (uint64_t)&_KernelStart;
    void* kernel_start = (void*)&_KernelStart;
    uint64_t kernel_pg_cnt = kernel_sz / PAGE_SIZE + 1;

    page_alloc_m(kernel_start, kernel_pg_cnt);

    uint64_t mem_size = mem_get_sz();
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

    asm volatile ("mov %0, %%cr3"::"r"(page_get_kpml4()));
}

void kernel_proc()
{
    DONE();

    // TESTS

    fs_node_t kb = vfs_resolve_path("/dev/keyboard", NULL);
    vfs_open(&kb);
    fs_node_t mouse = vfs_resolve_path("/dev/mouse", NULL);
    vfs_open(&mouse);

    fs_node_t* node = syscall(SYS_OPEN, "/system_folder/long_file_name.txt");

    for (;;)
    {
        mouse_packet_t pack;
        if (vfs_read(&mouse, &pack, 0, 1))
        {
            LOG("Mouse");
        }

        uint32_t key;
        if (vfs_read(&kb, &key, 0, 1))
        {
            char buffer[100];
            LOG(itoa(key, buffer, 10));
        }
    }
}

void _start(boot_info_t* inf)
{
    mem_boot_data(inf->mem_map, inf->mem_map_size / inf->mem_map_desc_size, inf->mem_map_desc_size);

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

    LOG("Initializing ACPI...");
    acpi_init(inf->rsdp->xsdt_addr);
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

    LOG("Initializing APIC...");
    apic_init();
    DONE();
    
    LOG("Creating kernel process...");
    proc_t* proc = mk_proc(kernel_proc);
    proc->addr_space = page_get_kpml4(); // Kernel process gets to use kernel pml4
    
    sched_spawn_proc(proc);
    DONE();

    // TEST
    fs_node_t node = vfs_resolve_path("/system_folder/executable.elf", NULL);
    vfs_open(&node);
    uint8_t* elf_dat = kmalloc(4904);
    vfs_read(&node, elf_dat, 0, 4904);
    vfs_close(&node);

    proc_t* elfproc = mk_elf_proc(elf_dat);
    //sched_spawn_proc(elfproc);
    
    LOG("Jumping to multitasking...");
    sched_init();

    for (;;);
}