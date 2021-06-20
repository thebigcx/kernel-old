make ARCH=x86_64_efi ARCHDIR=x86_64/efi ARCHTARGET=BOOTX64.EFI

dd if=/dev/zero of=bin/sata.img bs=1k count=14400

# FAT32
#mkdosfs -F32 bin/sata.img 14400
# EXT2
mkfs.ext2 bin/sata.img 1440

mkdir -p bin/sata
mount bin/sata.img bin/sata
cp -r base/* bin/sata
umount bin/sata
rm -rf bin/sata

mkdir -p bin
dd if=/dev/zero of=bin/os.img bs=1k count=1440
mformat -i bin/os.img -f 1440 ::
mkdir -p bin/fat
mount bin/os.img bin/fat
mkdir bin/fat/EFI
mkdir bin/fat/EFI/BOOT
cp boot/arch/x86_64/efi/BOOTX64.EFI bin/fat/EFI/BOOT
cp kernel/kernel.elf bin/fat
cp bin/font.psf bin/fat
umount bin/fat
rm -rf bin/fat

cd bin
mkdir -p iso
cp os.img iso
xorriso -as mkisofs -R -f -e os.img -no-emul-boot -o os.iso iso