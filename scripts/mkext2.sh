echo "Creating emulation SATA drive..."
dd if=/dev/zero of=bin/sata.img bs=1k count=1440
mkfs.ext2 bin/sata.img 1440
mkdir bin/sata
mount bin/sata.img bin/sata
cp -r bin/system_folder/* bin/sata
umount bin/sata
rm -rf bin/sata