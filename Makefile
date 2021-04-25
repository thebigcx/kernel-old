ISOTARGET := os.iso
FATTARGET := fat.img
EFITARGET := boot/BOOTX64.EFI
KERNEL := kernel/kernel.elf

all: $(ISOTARGET)

$(ISOTARGET): $(FATTARGET)
	cd bin
	mkdir -p iso
	cp bin/$< iso
	xorriso -as mkisofs -R -f -e $< -no-emul-boot -o bin/$@ iso

$(FATTARGET): $(EFITARGET)
	mkdir -p bin
	dd if=/dev/zero of=bin/$@ bs=1k count=1440
	mformat -i bin/$@ -f 1440 ::
	mmd -i bin/$@ ::/EFI
	mmd -i bin/$@ ::/EFI/BOOT
	mcopy -i bin/$@ $< ::/EFI/BOOT
	mcopy -i bin/$@ $(KERNEL) ::

$(EFITARGET):
	cd libc && make
	cd ..
	cd boot && make
	cd ..
	cd kernel && make
	cd ..

clean:
	rm -f bin/$(ISOTARGET)
	rm -f bin/$(FATTARGET)
	rm -rf bin/iso
	cd kernel && make clean
	cd ..
	cd boot && make clean
	cd ..
	cd libc && make clean
	cd ..

qemu:
	qemu-system-x86_64 -L /usr/share/OVMF/x64 -bios /usr/share/OVMF/x64/OVMF_CODE.fd -cdrom bin/$(ISOTARGET)