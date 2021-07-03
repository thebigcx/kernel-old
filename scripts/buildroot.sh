cp userland/apps/test/test base/bin

# Create the emulation disk TODO: partitions
dd if=/dev/zero of=dist/disk.img bs=1k count=14400
chown "$SUDO_UID":"$SUDO_GID" dist/disk.img
mkfs.ext2 dist/disk.img 1440
mkdir -p mnt
sudo mount dist/disk.img mnt
cp -r base/* mnt
sudo umount mnt
rm -rf mnt