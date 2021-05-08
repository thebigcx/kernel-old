ISOTARGET := os.iso
FATTARGET := fat.img
EFITARGET := boot/BOOTX64.EFI
KERNEL := kernel/kernel.elf
FONT := bin/font.psf

all: $(ISOTARGET)

$(ISOTARGET): $(FATTARGET)
	@echo "Creating CD image..."
	@cd bin
	@mkdir -p iso
	@cp bin/$< iso
	@xorriso -as mkisofs -R -f -e $< -no-emul-boot -o bin/$@ iso

$(FATTARGET): $(EFITARGET)
	@echo "Creating FAT image..."
	@mkdir -p bin
	@dd if=/dev/zero of=bin/$@ bs=1k count=1440
	@mformat -i bin/$@ -f 1440 ::
	@mmd -i bin/$@ ::/EFI
	@mmd -i bin/$@ ::/EFI/BOOT
	@mcopy -i bin/$@ $< ::/EFI/BOOT
	@mcopy -i bin/$@ $(KERNEL) ::
	@mcopy -i bin/$@ $(FONT) ::

	@echo "Creating emulation SATA drive..."
	@dd if=/dev/zero of=bin/sata.img bs=1k count=1440
	@mformat -i bin/sata.img -f 1440 ::

$(EFITARGET):
	@echo ""
	@echo "<========= Compiling libc =========>"
	@echo ""
	@cd libc && make
	@cd ..
	@echo ""
	@echo "<====== Compiling boot loader =====>"
	@echo ""
	@cd boot && make
	@cd ..
	@echo ""
	@echo "<========= Compiling kernel =======>"
	@echo ""
	@cd kernel && make
	@cd ..

clean:
	@echo "Cleaning iso..."
	@rm -f bin/$(ISOTARGET)
	@echo "Cleaning FAT image..."
	@rm -f bin/$(FATTARGET)
	@rm -rf bin/iso
	@echo "Cleaning kernel..."
	@cd kernel && make clean
	@cd ..
	@echo "Cleaning boot loader..."
	@cd boot && make clean
	@cd ..
	@echo "Cleaning libc..."
	@cd libc && make clean
	@cd ..

qemu:
	qemu-system-x86_64 -L /usr/share/OVMF/x64 -bios /usr/share/OVMF/x64/OVMF_CODE.fd -cdrom bin/$(ISOTARGET)