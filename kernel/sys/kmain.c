#include <util/types.h>
#include <drivers/gfx/fb/fb.h>
#include <mem/paging.h>
#include <mem/kheap.h>
#include <intr/idt.h>
#include <cpu/gdt.h>
#include <drivers/input/mouse/ps2_mouse.h>
#include <drivers/input/keyboard/ps2_keyboard.h>
#include <sys/io.h>
#include <time/time.h>
#include <acpi/acpi.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_ids.h>
#include <drivers/storage/ata/ahci/ahci.h>
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
#include <drivers/tty/serial.h>
#include <drivers/storage/partmgr/gpt.h>

void kernel_proc()
{
    //DONE(); // "Jumping to multitasking..."
    serial_writestr("Ok\n");
    //sched_block(PROC_STATE_PAUSED);
    //sleep(1);

    // TESTS
    
    vfs_node_t* kb = vfs_resolve_path("/dev/keyboard", NULL);
    vfs_open(kb, 0, 0);

    vfs_node_t* mouse = vfs_resolve_path("/dev/mouse", NULL);
    vfs_open(mouse, 0, 0);

    /*vfs_node_t test = vfs_resolve_path("/system_folder/long_file_name.txt", NULL);
    
    char buffer[100];

    vfs_read(&test, buffer, 0, 100);
    vfs_close(&test);

    for (int i = 0; i < 100; i++)
    {
        console_putchar(buffer[i], 255, 255, 255);
    }*/

    int x = 0;
    for (;;)
    {
        x += 1;

        for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
        {
            video_putpix(i + x, j, 255, 0, 0);
        }

        mouse_packet_t pack;
        if (vfs_read(mouse, &pack, 0, 1))
        {
            serial_writestr("Mouse");
        }

        uint32_t key;
        if (vfs_read(kb, &key, 0, 1))
        {
            serial_printf("%d", key);
        }
    }
}

void kmain()
{
    serial_writestr("Enumerating PCI devices...");
    pci_enumerate();
    serial_writestr("Ok\n");
    
    serial_writestr("Initializing AHCI controllers...");
    ahci_init(pci_devs);
    serial_writestr("Ok\n");

    serial_writestr("Initializing VFS...");
    vfs_init();
    serial_writestr("Ok\n");

    serial_writestr("Mounting /dev/disk0 to /...");
    //vfs_node_t* dev = gpt_getpart(ahci_get_dev(0), "Root");
    vfs_node_t* dev = ahci_get_dev(0);
    vfs_mount(dev, "/dev/disk0"); // Mount first disk

    vfs_node_t* root = ext2_init(dev);
    vfs_mount(root, "/"); // Mount root file system

    console_init();
    video_init();

    serial_writestr("Ok\n");

    vfs_node_t* test = vfs_resolve_path("/bin/test", NULL);
    vfs_open(test, 0, 0);

    uint8_t* elfdat = kmalloc(1024 - (test->size % 1024) + test->size);
    
    vfs_read(test, elfdat, 0, test->size);
    /*vfs_close(test);*/

    serial_writestr("Initializing keyboard...");
    kb_init();
    serial_writestr("Ok\n");
    serial_writestr("Initializing mouse...");
    mouse_init();
    serial_writestr("Ok\n");
    serial_writestr("Initializing random number generator...");
    rand_seed(305640980);
    serial_writestr("Ok\n");
    
    serial_writestr("Creating kernel process...");
    proc_t* proc = mk_proc(kernel_proc);
    sched_spawn_proc(proc);
    serial_writestr("Ok\n");

    proc_t* elfproc = mk_elf_proc(elfdat);
    sched_spawn_proc(elfproc);
    elfproc->next = proc;
    proc->next = elfproc;

    serial_writestr("Jumping to multitasking...");
    sched_init();

    for (;;);
}