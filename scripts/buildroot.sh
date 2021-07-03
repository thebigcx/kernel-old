cp userland/apps/test/test base/bin

# Create the emulation disk TODO: partitions
dd if=/dev/zero of=bin/sata.img bs=1k count=14400
chown "$SUDO_UID":"$SUDO_GID" bin/sata.img
mkfs.ext2 bin/sata.img 1440
mkdir -p bin/sata
sudo mount bin/sata.img bin/sata
cp -r base/* bin/sata
sudo umount bin/sata
rm -rf bin/sata