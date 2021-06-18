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
	@mkdir -p bin/fat
	@mount bin/$@ bin/fat
	@mkdir bin/fat/EFI
	@mkdir bin/fat/EFI/BOOT
	@cp $< bin/fat/EFI/BOOT
	@cp $(KERNEL) bin/fat
	@cp $(FONT) bin/fat
	@umount bin/fat
	@rm -rf bin/fat

$(EFITARGET):
	@echo ""
	@echo "<========= Compiling libraries ========>"
	@echo ""
	@cd lib && make
	@echo ""
	@echo "<====== Compiling boot loader =====>"
	@echo ""
	@cd boot && make
	@echo ""
	@echo "<========= Compiling kernel =======>"
	@echo ""
	@cd kernel && make

clean:
	@echo "Cleaning iso..."
	@rm -f bin/$(ISOTARGET)
	@rm -rf iso
	@echo "Cleaning FAT image..."
	@rm -f bin/$(FATTARGET)
	@rm -rf bin/iso
	@echo "Cleaning libraries..."
	@cd lib && make clean
	@echo "Cleaning kernel..."
	@cd kernel && make clean
	@echo "Cleaning boot loader..."
	@cd boot && make clean

qemu:
	qemu-system-x86_64 -L /usr/share/OVMF/x64 -bios /usr/share/OVMF/x64/OVMF_CODE.fd -cdrom bin/$(ISOTARGET)