make -j4 ARCH=x86_64_bios ARCHDIR=x86_64/bios ARCHTARGET=boot.o

dd if=/dev/zero of=bin/sata.img bs=1k count=1440
mkfs.ext2 bin/sata.img 1440
mkdir bin/sata
mount bin/sata.img bin/sata
cp -r base/* bin/sata
umount bin/sata
rm -rf bin/sata

mkdir -p bin
mkdir -p bin/iso
mkdir -p bin/iso/boot
mkdir -p bin/iso/boot/grub
cp kernel/kernel.elf bin/iso/boot
cp boot/arch/x86_64/bios/grub.cfg bin/iso/boot/grub
grub-mkrescue -o os.iso bin/iso

cd bin
mkdir -p iso
cp os.img iso
xorriso -as mkisofs -R -f -e os.img -no-emul-boot -o os.iso iso