#include <util/types.h>
#include <drivers/gfx/fb/fb.h>
#include <mem/paging.h>
#include <mem/kheap.h>
#include <intr/idt.h>
#include <arch/x86_64/gdt.h>
#include <drivers/input/mouse/ps2mouse.h>
#include <drivers/input/keyboard/ps2kb.h>
#include <sys/io.h>
#include <time/time.h>
#include <acpi/acpi.h>
#include <drivers/pci/pci.h>
#include <drivers/pci/pci_ids.h>
#include <drivers/storage/ata/ahci/ahci.h>
#include <fs/vfs/vfs.h>
#include <sched/sched.h>
#include <intr/apic.h>
#include <sys/syscall/syscall.h>
#include <sys/console.h>
#include <util/rand.h>
#include <arch/x86_64/smp.h>
#include <intr/pic.h>
#include <util/bmp.h>
#include <util/stdlib.h>
#include <util/elf.h>
#include <drivers/tty//serial/serial.h>
#include <drivers/storage/partmgr/gpt.h>
#include <drivers/net/e1000/e1000.h>

void kernel_proc()
{
    serial_printf("Ok\n");

    // Clean terminated processes, wake up sleeping processes when they're done
    for (;;)
    {
        /*list_t* procs = sched_getprocs();

        for (uint32_t i = 0; i < procs->cnt; i++)
        {
            proc_t* proc = list_get(procs, i)->val;

            if (proc->state == PROC_STATE_KILLED)
            {
                sched_proc_destroy(proc);
                list_remove(procs, i);
            }

            if (proc->state == PROC_STATE_SLEEP)
            {
                if (proc->sleep_exp < pit_uptime() * 1000)
                {
                    proc->state = PROC_STATE_READY;
                    sched_spawn(proc, NULL);
                }
            }
        }*/
		continue;

        for (uint32_t i = 0; i < cpu_count; i++)
        {
            cpu_t* cpu = &cpus[i];
			serial_printf("ACQUIRE %d\n", cpu->lapic_id);
            acquire_lock(cpu->lock);
			serial_printf("DONE %d\n", cpu->lapic_id);

            for (uint32_t i = 0; i < cpu->threads->cnt; i++)
            {
                thread_t* thread = list_get(cpu->threads, i)->val;

                if (thread->state == THREAD_STATE_KILLED)
                {
                    list_remove(cpu->threads, i);
                    //sched_proc_destroy(proc);
                    //list_remove(procs, i);
                }

                if (thread->state == THREAD_STATE_ASLEEP)
                {
                    if (thread->sleepexp <= timer_uptime())
                    {
                        //serial_printf("%d, %d\n", thread->sleepexp, pit_uptime());
                        thread->state = THREAD_STATE_READY;
						thread_spawn(thread);
                        //sched_spawn(proc, NULL);
                    }
                }
            }

            release_lock(cpu->lock);
			serial_printf("RELEASE %d\n", cpu->lapic_id);
        }

    }
}

void test1()
{
    for (;;);
}

void test2()
{
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

    serial_writestr("Initializing network adapter...");
    list_foreach(pci_devs, node)
    {
        pci_dev_t* dev = node->val;
        if (dev->class_code == PCI_CLASS_NET && dev->subclass == PCI_SC_ETHERNET_CTRL)
        {
            e1000_init(dev);
        }
    }
    serial_writestr("\n");

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

    serial_writestr("Creating kernel process...");
    sched_spawn(sched_mkproc(kernel_proc), NULL);
    serial_writestr("Ok\n");

    sched_spawninit();
    serial_writestr("Intializing scheduler...");
    sched_start();

    for (;;);
}
