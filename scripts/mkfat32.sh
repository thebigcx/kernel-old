echo "Creating emulation SATA drive..."
dd if=/dev/zero of=bin/sata.img bs=1k count=1440
mkdosfs -F32 bin/sata.img 1440
mkdir -p bin/sata
mount bin/sata.img bin/sata
cp -r bin/system_folder/* bin/sata
umount bin/sata
rm -rf bin/sata