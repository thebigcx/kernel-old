#include <util/types.h>
#include <drivers/video/video.h>
#include <mem/paging.h>
#include <mem/mem.h>
#include <mem/heap.h>
#include <intr/idt.h>
#include <cpu/gdt.h>
#include <drivers/input/mouse/ps2_mouse.h>
#include <drivers/input/keyboard/ps2_keyboard.h>
#include <sys/io.h>
#include <time/time.h>
#include <acpi/acpi.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_ids.h>
#include <drivers/storage/ahci.h>
#include <drivers/fs/vfs/vfs.h>
#include <sched/sched.h>
#include <intr/apic.h>
#include <sys/syscall/syscall.h>
#include <sys/console.h>
#include <util/rand.h>
#include <cpu/smp.h>
#include <intr/pic.h>
#include <util/bmp.h>
#include <util/stdlib.h>
#include <util/elf.h>

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

void test_proc()
{
    sched_block(PROC_STATE_WAITING);
    console_printf("TEST", 255, 255, 255);
    for(;;);
}

void kernel_proc()
{
    DONE(); // "Jumping to multitasking..."
    //sched_block(PROC_STATE_PAUSED);
    //sleep(1);

    // TESTS
    
    vfs_node_t* kb = vfs_resolve_path("/dev/keyboard", NULL);
    vfs_open(kb, 0);

    vfs_node_t* mouse = vfs_resolve_path("/dev/mouse", NULL);
    vfs_open(mouse, 0);

    /*vfs_node_t test = vfs_resolve_path("/system_folder/long_file_name.txt", NULL);
    
    char buffer[100];

    vfs_read(&test, buffer, 0, 100);
    vfs_close(&test);

    for (int i = 0; i < 100; i++)
    {
        console_putchar(buffer[i], 255, 255, 255);
    }*/

    float x = 0;
    for (;;)
    {
        x += 0.2;

        for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
        {
            video_putpix(i + x, j, 255, 0, 0);
        }

        mouse_packet_t pack;
        if (vfs_read(mouse, &pack, 0, 1))
        {
            LOG("Mouse");
        }

        uint32_t key;
        if (vfs_read(kb, &key, 0, 1))
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

    cli();

    LOG("Initializing GDT...");
    gdt_desc.size = sizeof(gdt_t) - 1;
    gdt_desc.offset = (uint64_t)&gdt_def;
    gdt_load(&gdt_desc);
    DONE();

    LOG("Initializing paging and memory...");
    init_paging(inf);
    DONE();
    LOG("Initializing heap...");
    heap_init();
    DONE();

    LOG("Loading IDT...");
    idt_load();
    pic_init();
    DONE();

    LOG("Initializing PIT...");
    pit_init(1000);
    DONE();

    sti();

    LOG("Initializing ACPI...");
    acpi_init(inf->rsdp);
    DONE();

    LOG("Initializing APIC...");
    apic_init();
    DONE();

    LOG("Initializing SMP...");
    smp_init();
    DONE();

    LOG("Enumerating PCI devices...");
    pci_enumerate();
    DONE();

    cli();
    
    LOG("Initializing AHCI controllers...");
    ahci_init(&pci_devices);
    DONE();

    LOG("Mounting /dev/disk0 to /...");
    vfs_init();

    vfs_node_t* dev = ahci_get_dev(0);
    vfs_mount(dev, "/dev/disk0"); // Mount first disk

    vfs_node_t* root = ext2_init(dev);
    vfs_mount(root, "/"); // Mount root file system

    console_init();
    
    /*vfs_node_t* text = vfs_resolve_path("/text/test.txt", NULL);
    vfs_open(text, 0);
    char buf[1024];
    memset(buf, 'L', 1024);
    vfs_write(text, buf, 0, 1024);

    char buf2[1024];
    vfs_read(text, buf2, 0, 1024);
    for (int i = 0; i < 1024; i++)
    {
        console_putchar(buf2[i], 255, 255, 255);
    }
    vfs_close(text);
    breakpoint();
    
    vfs_node_t* icon = vfs_resolve_path("/images/bmp24tst.bmp", NULL);
    vfs_open(icon, 0);
    size_t size = icon->size;

    uint8_t* buffer = kmalloc(1000000);
    
    vfs_read(icon, buffer, 0, size);
    //vfs_close(&icon);

    int w, h;
    uint8_t* data = bmp_load(buffer, &w, &h);
    kfree(buffer);

    //video_draw_img(0, 0, w, h, data);

    kfree(data);*/

    DONE();

    cli();

    vfs_node_t* test = vfs_resolve_path("/bin/test", NULL);
    vfs_open(test, 0);

    uint8_t* buf = kmalloc(512 - (test->size % 512) + test->size);
    
    vfs_read(test, buf, 0, test->size);
    /*vfs_close(test);*/

    LOG("Initializing keyboard...");
    kb_init();
    DONE();
    LOG("Initializing mouse...");
    mouse_init();
    DONE();
    LOG("Initializing random number generator...");
    rand_seed(305640980);
    DONE();
    
    LOG("Creating kernel process...");
    proc_t* proc = mk_proc(kernel_proc);
    sched_spawn_proc(proc);
    DONE();

    proc_t* elfproc = mk_elf_proc(buf);
    sched_spawn_proc(elfproc);
    elfproc->next = proc;
    proc->next = elfproc;
    
    LOG("Jumping to multitasking...");
    sched_init();

    for (;;);
}