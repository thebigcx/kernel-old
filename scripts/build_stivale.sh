if [ ! -d ./limine ]; then
        git clone https://github.com/limine-bootloader/limine.git --branch=v2.0-branch-binary --depth=1
        make -C limine
fi

make
mkdir -p iso
cp -v kernel/kernel.elf kernel/arch/x86_64/limine.cfg limine/limine.sys limine/limine-cd.bin limine/limine-eltorito-efi.bin iso
mkdir -p dist
xorriso -as mkisofs -b limine-cd.bin \
        -no-emul-boot -boot-load-size 4 -boot-info-table \
        --efi-boot limine-eltorito-efi.bin \
        -efi-boot-part --efi-boot-image --protective-msdos-label \
        iso -o dist/image.iso

./limine/limine-install dist/image.iso

