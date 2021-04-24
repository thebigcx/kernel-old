#include <efi.h>
#include <efilib.h>

#include "../kernel/drivers/graphics/graphics.h"

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);

    EFI_STATUS status;

    init_graphics();

    // Exit boot services

    UINTN map_size, map_key, descriptor_size;
    UINT32 descriptor_version;

    EFI_MEMORY_DESCRIPTOR* memory_map = (EFI_MEMORY_DESCRIPTOR*)uefi_call_wrapper(LibMemoryMap, 4, &map_size, &map_key, &descriptor_size, &descriptor_version);
    status = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2, ImageHandle, map_key);

    for (int x = 0; x < 100; x++)
    for (int y = 0; y < 100; y++)
    {
        *((uint32_t*)(graphics_data.fb_adr + 4 * graphics_data.pix_per_line * x + 4 * y)) = 0x00FF0000;
    }
    

    while (1);
    return EFI_SUCCESS;
}