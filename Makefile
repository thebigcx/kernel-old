EFITARGET := boot/BOOTX64.EFI
KERNEL := kernel/kernel.elf
FONT := bin/font.psf

ARCH ?=
ARCHDIR ?=
ARCHTARGET ?=

all:
	@echo ""
	@echo "<========= Compiling libraries ========>"
	@echo ""
	@cd lib && make
	@echo ""
	@echo "<======== Compiling bootloader ========>"
	@echo ""
	@cd boot/arch/$(ARCHDIR) && make
	@echo ""
	@echo "<========== Compiling kernel ==========>"
	@echo ""
	@cd kernel && make

clean:
	@echo "Cleaning iso..."
	@rm -f bin/os.iso
	@rm -rf iso
	@echo "Cleaning FAT image..."
	@rm -f bin/os.img
	@rm -f bin/sata.img
	@rm -rf bin/iso
	@echo "Cleaning libraries..."
	@cd lib && make clean
	@echo "Cleaning kernel..."
	@cd kernel && make clean
	@echo "Cleaning boot loader..."
	@cd boot/arch/$(ARCHDIR) && make clean