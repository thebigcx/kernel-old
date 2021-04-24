#include "graphics.h"

#include <efi.h>
#include <efilib.h>

graphics_data_t graphics_data;

void gr_init()
{
    EFI_STATUS status;

    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    status = uefi_call_wrapper(BS->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);
    if(EFI_ERROR(status))
        Print(L"Unable to locate GOP\n");

    EFI_GRAPHICS_OUTPUT_MODE_INFORMATION* info;
    UINTN size_of_info, num_modes, native_mode;

    status = uefi_call_wrapper(gop->QueryMode, 4, gop, gop->Mode == NULL ? 0 : gop->Mode->Mode, &size_of_info, &info);

    if (EFI_ERROR(status))
        Print(L"Unable to get native mode\n");

    native_mode = gop->Mode->Mode;
    num_modes = gop->Mode->MaxMode;

    uefi_call_wrapper(gop->SetMode, 2, gop, num_modes - 1);

    graphics_data.fb_adr = gop->Mode->FrameBufferBase;
    graphics_data.pix_per_line = gop->Mode->Info->PixelsPerScanLine;
    graphics_data.v_res = gop->Mode->Info->VerticalResolution;
}

void gr_clear(float r, float g, float b, float a)
{
    for (uint32_t x = 0; x < graphics_data.pix_per_line; x++)
    for (uint32_t y = 0; y < graphics_data.v_res; y++)
    {   
        *((uint32_t*)(graphics_data.fb_adr + 4 * graphics_data.pix_per_line * y + 4 * x)) = 0xFFFFFFFF;
    }
}