@echo -off
mode 80 25

cls

if exist .\efi\boot\BOOTX64.EFI then
    .\efi\boot\BOOTX64.EFI
    goto END
endifi

if exist fs0:\efi\boot\BOOTX64.EFI then
    fs0:
    echo Found bootloader in fs0:
    efi\boot\BOOTX64.EFI
    goto END
endif