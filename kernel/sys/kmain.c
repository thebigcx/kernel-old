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

sem_t* mutex;

void kernel_proc()
{
    serial_printf("Ok\n");
    /*mutex_acquire(mutex);

    int x = 0;
    for (;;)
    {
        if (x > 500)
        {
            mutex_release(mutex);
        }
        x += 1;

        for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
        {
            video_putpix(i + x, j, 255, 0, 0);
        }
    }*/
    for (;;);
}

void kernel_proc2()
{
    /*mutex_acquire(mutex);
    int y = 0;
    for (;;)
    {
        y += 1;

        for (int i = 0; i < 100; i++)
        for (int j = 0; j < 100; j++)
        {
            video_putpix(i, j + y, 255, 255, 0);
        }
    }*/
    for (;;);
}

// TODO: only load drivers for devices if they are present. This should
// take the form of kernel modules, and the initializing should not EXPECT
// a particular device to be present.

void kmain()
{
    serial_writestr("Enumerating PCI devices...");
    pci_enumerate();
    serial_writestr("Ok\n");

    serial_writestr("Initializing VFS...");
    vfs_init();
    serial_writestr("Ok\n");
    

//                  Everything here is temporary
// -----------------------------------------------------------------

    serial_writestr("Initializing AHCI controllers...");
    ahci_init(pci_devs);
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

// ---------------------------------------------------------------------

    serial_writestr("Initializing keyboard...");
    kb_init();
    serial_writestr("Ok\n");

    serial_writestr("Initializing mouse...");
    mouse_init();
    serial_writestr("Ok\n");

    serial_writestr("Initializing random number generator...");
    rand_seed(305640980);
    serial_writestr("Ok\n");

    sched_init();
    
    serial_writestr("Creating kernel process...");
    sched_spawn(mk_proc(kernel_proc), NULL);
    sched_spawn(mk_proc(kernel_proc2), NULL);
    serial_writestr("Ok\n");

    // TEMP
    const char* hello = "Hello, this is the first parameter!";
    proc_t* elf = mkelfproc("/bin/test", 1, &hello, 0, NULL);
    sched_spawn(elf, NULL);
    mutex = mutex_create();

    serial_writestr("Intializing scheduler...");
    sched_start();

    for (;;);
}