SOURCES := $(shell find ./ -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))

EFITARGET := BOOTX64.EFI
EFILIB := /usr/local/lib
EFILDS := $(EFILIB)/elf_x86_64_efi.lds
EFICRT := $(EFILIB)/crt0-efi-x86_64.o
EFIINC := -I/usr/include/efi

CFLAGS := -ffreestanding -fshort-wchar -O2 -fpic
LDFLAGS := -shared -Bsymbolic -L$(EFILIB) -T$(EFILDS) $(EFICRT) -nostdlib

bootloader: $(EFITARGET)

%.o: %.c
	gcc $(CFLAGS) $(EFIINC) -c -o $@ $<

main.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

$(EFITARGET): main.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $^ $@
	rm main.so

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(EFITARGET)