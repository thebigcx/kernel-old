#include <arch/x86_64/hal.h>
#include <drivers/gfx/fb/fb.h>
#include <drivers/tty//serial/serial.h>
#include <arch/x86_64/system.h>
#include <intr/idt.h>
#include <mem/pmm.h>
#include <mem/paging.h>
#include <acpi/acpi.h>
#include <drivers/tty//serial/serial.h>

void init_core()
{
    serial_init();

    serial_writestr("Initializing BSP GDT...");
    gdt_init();
    serial_writestr("Ok\n");

    serial_writestr("Initializing IDT...");
    idt_init();
    pic_init();
    serial_writestr("Ok\n");

    serial_writestr("Initializing memory...");
    pmm_init(512 * 1000000); // TODO: detect memory
    paging_init();

    serial_writestr("Ok\n");

    cli();
}

void init_extra()
{
    serial_writestr("Initializing kernel heap...");
    kheap_init();
    serial_writestr("Ok\n");

    pit_init(1000);

    serial_writestr("Parsing ACPI tables...");
    acpi_init();
    serial_writestr("Ok\n");

    serial_writestr("Initializing the BSP local and IO APIC...");
    apic_init();
    serial_writestr("Ok\n");

    serial_writestr("Initializing SMP and attempting to initialize other CPUs...\n");
    smp_init();
    serial_writestr("Ok\n");
}

void init_stivale2(st2_struct_t* st2)
{
    uint64_t tags_phys = st2->tags;

    init_core();

    for (st2_tag_t* tag = tags_phys; tag != NULL; tag = tag->next)
    {
        switch (tag->id)
        {
            case STIVALE2_STRUCT_TAG_FRAMEBUFFER_ID:
            {
                // Create and set generic framebuffer driver

                serial_writestr("Found framebuffer tag\n");
                st2_fbinfo_t* fb = (st2_fbinfo_t*)tag;

                uint32_t pagecnt = (fb->width * fb->height * (fb->depth / 8)) / PAGE_SIZE_4K;
                void* virtaddr = page_kernel_alloc4k(pagecnt);
                page_kernel_map_memory(virtaddr, fb->addr, pagecnt);

                vid_mode_t vidmode =
                {
                    .width = fb->width,
                    .height = fb->height,
                    .depth = fb->depth,
                    .fb = virtaddr,
                    .fbphys = fb->addr
                };

                video_setmode(vidmode);

                break;
            }

            case STIVALE2_STRUCT_TAG_RSDP_ID:
            {
                // Set ACPI RSDP address

                serial_writestr("Found ACPI RSDP tag\n");
                st2_tagrsdp_t* rsdptag = (st2_tagrsdp_t*)tag;
                acpi_setrsdp(rsdptag->rsdp);
                break;
            }

            case STIVALE2_STRUCT_TAG_MEMMAP_ID:
            {
                // Parse memory map

                serial_writestr("Found memory map tag\n");
                st2_tagmmap_t* mmaptag = (st2_tagmmap_t*)tag;
                uint64_t memsz = 0;

                for (uint32_t i = 0; i < mmaptag->entries; i++)
                {
                    st2_mmap_ent_t* ent = &mmaptag->memmap[i];
                    memsz += ent->length;
                    
                    switch (ent->type)
                    {
                        case STIVALE2_MMAP_USABLE:
                        case STIVALE2_MMAP_BOOTLOADER_RECLAIMABLE:
                            //serial_printf("Reclaiming %d - %d\n", ent->base, ent->base + ent->length);
                            pmm_release_m(ent->base, ent->length / PAGE_SIZE_4K);
                            break;
                        
                        default:
                            break;
                    }
                }

                serial_printf("Detected memory: %d B (%d MB)\n", memsz, memsz / 1048576);
                break;
            }

            default:
            {
                serial_writestr("Found unrecognised tag\n");
                break;
            }
        }
    }

    pmm_reserve_m(0, 0x10); // For SMP
    
    sti();
    init_extra();
    kmain();
}
