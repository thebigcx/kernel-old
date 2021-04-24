#include <efi.h>
#include <efilib.h>

#include "../kernel/drivers/graphics/graphics.h"
#include "boot.h"

boot_state_t boot_state;

static void init_mem_map(EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS status;
    /*boot_state.mem_map = (void*)uefi_call_wrapper(LibMemoryMap, 4, &boot_state.mem_map_size,
                                                                   &boot_state.mem_map_key,
                                                                   &boot_state.mem_map_desc_size,
                                                                   &boot_state.mem_map_desc_vers);*/

    boot_state.mem_map = NULL;

    {
        status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &boot_state.mem_map_size, boot_state.mem_map, &boot_state.mem_map_key, &boot_state.mem_map_desc_size, &boot_state.mem_map_desc_vers);
        //boot_state.mem_map_size = 10000;
        boot_state.mem_map_size += 2 * boot_state.mem_map_desc_size;
        status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, boot_state.mem_map_size, (void**)&boot_state.mem_map);
        status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &boot_state.mem_map_size, boot_state.mem_map, &boot_state.mem_map_key, &boot_state.mem_map_desc_size, &boot_state.mem_map_desc_vers);
    }

    // Print memory

    /*uint64_t map_entries = boot_state.mem_map_size / boot_state.mem_map_desc_size;
    Print(L"Map entries: %d\n", map_entries);

    for (int i = 0; i < 20; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)(boot_state.mem_map + i * boot_state.mem_map_desc_size);
        Print(L"Size: %dK Type: %d\n", desc->NumberOfPages * 4096 / 1024, desc->Type);
    }*/
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    uefi_call_wrapper(SystemTable->BootServices->SetWatchdogTimer, 4, 0, 0, 0, NULL);

    EFI_STATUS status;

    gr_init();

    init_mem_map(SystemTable);

    // Exit boot services

    status = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2, ImageHandle, boot_state.mem_map_key);
    if (EFI_ERROR(status))
        Print(L"Error (%d): ExitBootServices\n", status);

    SystemTable->RuntimeServices->SetVirtualAddressMap(boot_state.mem_map_size, boot_state.mem_map_desc_size, boot_state.mem_map_desc_vers, boot_state.mem_map);

    int px = 0;
    //while (1)
    {
        gr_clear(0, 0, 0, 1);
        for (int x = 0; x < 100; x++)
        for (int y = 0; y < 100; y++)
        {
            *((uint32_t*)(graphics_data.fb_adr + 4 * graphics_data.pix_per_line * (x + px) + 4 * y)) = 0x00428f56;
        }

        px++;
    }
    

    while (1);
    return EFI_SUCCESS;
}