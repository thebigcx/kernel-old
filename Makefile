CSOURCES := $(wildcard *.c) $(wildcard **/*.c) $(wildcard **/**/*.c) $(wildcard **/**/*.c) $(wildcard **/**/**/*.c) $(wildcard **/**/**/**/*.c)
OBJS := $(patsubst %.c, %.o, $(CSOURCES))
ISOTARGET := os.iso
FATTARGET := fat.img
EFITARGET := BOOTX64.EFI
EFILIB := /usr/local/lib
EFILDS := $(EFILIB)/elf_x86_64_efi.lds
EFICRT := $(EFILIB)/crt0-efi-x86_64.o
EFIINC := -I/usr/include/efi

CFLAGS := $(EFIINC) -fpic -ffreestanding -fno-stack-protector -fno-stack-check -fshort-wchar -mno-red-zone -maccumulate-outgoing-args -c -Ilibc/include -O2
LDFLAGS := -shared -Bsymbolic -L$(EFILIB) -T$(EFILDS) $(EFICRT) -nostdlib

all: $(ISOTARGET)

$(ISOTARGET): $(FATTARGET)
	mkdir -p iso
	cp $< iso
	xorriso -as mkisofs -R -f -e $< -no-emul-boot -o $@ iso

$(FATTARGET): $(EFITARGET)
	dd if=/dev/zero of=$@ bs=1k count=1440
	mformat -i $@ -f 1440 ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $< ::/EFI/BOOT

%.o: %.c
	gcc $(CFLAGS) -o $@ $<

main.so: $(OBJS)
	ld $(LDFLAGS) $(OBJS) -o $@ -lefi -lgnuefi

$(EFITARGET): main.so
	objcopy -j .text -j .sdata -j .data -j .dynamic -j .dynsym  -j .rel -j .rela -j .rel.* -j .rela.* -j .reloc --target efi-app-x86_64 --subsystem=10 $^ $@

clean:
	rm $(OBJS)
	rm $(ISOTARGET)
	rm $(FATTARGET)
	rm $(EFITARGET)
	rm *.so