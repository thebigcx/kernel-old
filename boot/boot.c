#include <efi.h>
#include <efilib.h>
#include <elf.h>

#include "../kernel/drivers/graphics/graphics.h"
#include "../kernel/kernel/memory.h"
#include "boot.h"

boot_info_t boot_inf;

static void init_mem_map(EFI_SYSTEM_TABLE* SystemTable)
{
    EFI_STATUS status;

    boot_inf.mem_map = NULL;

    {
        status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &boot_inf.mem_map_size, boot_inf.mem_map, &boot_inf.mem_map_key, &boot_inf.mem_map_desc_size, &boot_inf.mem_map_desc_vers);
        boot_inf.mem_map_size += 2 * boot_inf.mem_map_desc_size;
        status = uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, boot_inf.mem_map_size, (void**)&boot_inf.mem_map);
        status = uefi_call_wrapper(SystemTable->BootServices->GetMemoryMap, 5, &boot_inf.mem_map_size, boot_inf.mem_map, &boot_inf.mem_map_key, &boot_inf.mem_map_desc_size, &boot_inf.mem_map_desc_vers);
    }

    uint64_t map_entries = boot_inf.mem_map_size / boot_inf.mem_map_desc_size;
    memory_map.address_cnt = 0;

    for (int i = 0; i < 20; i++)
    {
        EFI_MEMORY_DESCRIPTOR* desc = (EFI_MEMORY_DESCRIPTOR*)(boot_inf.mem_map + i * boot_inf.mem_map_desc_size);
        if (desc->Type == EfiConventionalMemory)
        {
            memory_map.addresses[memory_map.address_cnt++] = (void*)desc->PhysicalStart;
        }
    }
}

static void init_gop(EFI_SYSTEM_TABLE* sys_table)
{
    EFI_STATUS status;

    EFI_GRAPHICS_OUTPUT_PROTOCOL* gop;
    EFI_GUID gop_guid = EFI_GRAPHICS_OUTPUT_PROTOCOL_GUID;
    status = uefi_call_wrapper(sys_table->BootServices->LocateProtocol, 3, &gop_guid, NULL, (void**)&gop);
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

    boot_inf.fb_adr = gop->Mode->FrameBufferBase;
    boot_inf.pix_per_line = gop->Mode->Info->PixelsPerScanLine;
    boot_inf.v_res = gop->Mode->Info->VerticalResolution;
}

static EFI_FILE* load_file(EFI_FILE* dir, CHAR16* path, EFI_HANDLE handle, EFI_SYSTEM_TABLE* sys_table)
{
    EFI_FILE* file;

    EFI_LOADED_IMAGE_PROTOCOL* loaded_image;
    uefi_call_wrapper(sys_table->BootServices->HandleProtocol, 3, handle, &gEfiLoadedImageProtocolGuid, (void**)&loaded_image);

    EFI_SIMPLE_FILE_SYSTEM_PROTOCOL* fs;
    uefi_call_wrapper(sys_table->BootServices->HandleProtocol, 3, loaded_image->DeviceHandle, &gEfiSimpleFileSystemProtocolGuid, (void**)&fs);

    if (dir == NULL)
    {
        uefi_call_wrapper(fs->OpenVolume, 2, fs, &dir);
    }

    EFI_STATUS status = uefi_call_wrapper(dir->Open, 5, dir, &file, path, EFI_FILE_MODE_READ, EFI_FILE_READ_ONLY);
    if (EFI_ERROR(status))
    {
        Print(L"Error loading file\n");
        return NULL;
    }

    return file;
}

EFI_STATUS EFIAPI efi_main(EFI_HANDLE ImageHandle, EFI_SYSTEM_TABLE* SystemTable)
{
    InitializeLib(ImageHandle, SystemTable);
    
    // Load kernel
    Print(L"Loading kernel\n");
    EFI_FILE* kernel = load_file(NULL, L"kernel.elf", ImageHandle, SystemTable);

    Elf64_Ehdr header;
    {
        UINTN file_inf_size;
        EFI_FILE_INFO* inf;
        uefi_call_wrapper(kernel->GetInfo, 4, kernel, &gEfiFileInfoGuid, &file_inf_size, NULL);
        uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, file_inf_size, (void**)&inf);
        uefi_call_wrapper(kernel->GetInfo, 4, kernel, &gEfiFileInfoGuid, &file_inf_size, (void**)&inf);

        UINTN size = sizeof(header);
        uefi_call_wrapper(kernel->Read, 3, kernel, &size, &header);
    }

    if (
        header.e_ident[EI_CLASS] != ELFCLASS64 ||
		header.e_ident[EI_DATA] != ELFDATA2LSB ||
		header.e_type != ET_EXEC ||
		header.e_machine != EM_X86_64 ||
		header.e_version != EV_CURRENT
    )
    {
        Print(L"Kernel format bad!\n");
    }
    else
    {
        Print(L"Kernel header successfully verified\n");
    }

    Elf64_Phdr* phdrs;
    {
        uefi_call_wrapper(kernel->SetPosition, 2, kernel, header.e_phoff);
        UINTN size = header.e_phnum * header.e_phentsize;
        uefi_call_wrapper(SystemTable->BootServices->AllocatePool, 3, EfiLoaderData, size, (void**)&phdrs);
        uefi_call_wrapper(kernel->Read, 3, kernel, &size, phdrs);
    }

    for (
        Elf64_Phdr* phdr = phdrs;
        (char*)phdr < (char*)phdrs + header.e_phnum * header.e_phentsize;
        phdr = (Elf64_Phdr*)((char*)phdr + header.e_phentsize)
    )
    {
        switch (phdr->p_type)
        {
            case PT_LOAD:
            {
                int pages = (phdr->p_memsz + 0x1000 - 1) / 0x1000;
                Elf64_Addr segment = phdr->p_paddr;
                uefi_call_wrapper(SystemTable->BootServices->AllocatePages, 4, AllocateAddress, EfiLoaderData, pages, &segment);

                uefi_call_wrapper(kernel->SetPosition, 2, kernel, phdr->p_offset);
                UINTN size = phdr->p_filesz;
                uefi_call_wrapper(kernel->Read, 3, kernel, &size, (void*)segment);
                break;
            }
        }
    }

    EFI_STATUS status;

    Print(L"Intializing graphics\n");
    init_gop(SystemTable);

    Print(L"Initalizing memory map\n");
    init_mem_map(SystemTable);

    // Exit boot services

    status = uefi_call_wrapper(SystemTable->BootServices->ExitBootServices, 2, ImageHandle, boot_inf.mem_map_key);
    if (EFI_ERROR(status))
        Print(L"Error (%d): ExitBootServices\n", status);

    SystemTable->RuntimeServices->SetVirtualAddressMap(boot_inf.mem_map_size, boot_inf.mem_map_desc_size, boot_inf.mem_map_desc_vers, boot_inf.mem_map);

    void (*kernel_start)() = ((__attribute__((sysv_abi)) void (*)()) header.e_entry);
    kernel_start();
    
    return EFI_SUCCESS;
}