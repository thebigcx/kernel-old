#include <arch/x86_64/hal.h>
#include <drivers/gfx/fb/fb.h>
#include <drivers/tty/serial.h>
#include <sys/system.h>
#include <intr/idt.h>
#include <mem/pmm.h>
#include <mem/paging.h>
#include <acpi/acpi.h>

void init_core()
{
    serial_init();

    serial_writestr("Initializing IDT...");
    idt_init();
    pic_init();
    serial_writestr("Ok\n");

    serial_writestr("Initializing memory...");
    gdt_init();
    pmm_init(512 * 1000000); // TODO: detect memory
    paging_init();
    serial_writestr("Ok\n");

    cli();
}

void init_extra()
{
    serial_writestr("Initializing kernel heap...");
    heap_init();
    serial_writestr("Ok\n");

    pit_init(1000);

    serial_writestr("Parsing ACPI tables...");
    acpi_init();
    serial_writestr("Ok\n");

    /*serial_writestr("Initializing the BSP local and IO APIC...");
    apic_init();
    serial_writestr("Ok\n");

    serial_writestr("Initializing SMP and attempting to initialize other CPUs...");
    smp_init();
    serial_writestr("Ok\n");*/
}

void init_stivale2(st2_struct_t* st2)
{
    init_core();

    for (st2_tag_t* tag = (void*)st2->tags; tag != NULL; tag = tag->next)
    {
        serial_writestr("Found tag\n");

        switch (tag->id)
        {
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
            {
                st2_fbinfo_t* fb = (st2_fbinfo_t*)tag;
                // Create and set generic driver

                void* virtaddr = fb->addr; // TODO: virtual memory

                vid_mode_t vidmode =
                {
                    .width = fb->width,
                    .height = fb->height,
                    .depth = fb->depth,
                    .fb = virtaddr
                };

                video_setmode(vidmode);

                break;
            }

            case STIVALE2_STRUCT_TAG_RSDP_ID:
            {
                st2_tagrsdp_t* rsdptag = (st2_tagrsdp_t*)tag;
                // Set ACPI RSDP address
                acpi_setrsdp(rsdptag->rsdp);
                break;
            }

            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
            {
                st2_tagmmap_t* mmaptag = (st2_tagmmap_t*)tag;
                uint64_t memsz = 0;
                // Parse memory map

                for (uint32_t i = 0; i < mmaptag->entries; i++)
                {
                    st2_mmap_ent_t* ent = &mmaptag->memmap[i];
                    memsz += ent->length;
                    
                    switch (ent->type)
                    {
                        case STIVALE2_MMAP_USABLE:
                        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                            //serial_printf("Region %d - %d is free.\n", ent->base, ent->base + ent->length);
                            //serial_writestr("Region %d - %d is free.\n");
                            //serial_write('c');
                            serial_writestr("Reclaiming region\n");
                            pmm_release_m(ent->base, ent->length / PAGE_SIZE);
                            break;
                        
                        default:
                            break;
                    }
                }

                char buf[20];
                serial_writestr("Detected memory: ");
                serial_writestr(itoa(memsz, buf, 10));
                serial_write('\n');

                break;
            }
        }
    }

    pmm_reserve_m(0, 0x100);

    init_extra();

    kmain();
}