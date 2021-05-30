echo "Creating emulation SATA drive..."
dd if=/dev/zero of=bin/sata.img bs=1k count=1440
mkdosfs -F32 bin/sata.img 1440
mcopy -i bin/sata.img bin/system_folder ::