dd if=/dev/zero of=bin/disk.img bs=1M count=20
dev = $(losetup --find --partscan --show disk.img)