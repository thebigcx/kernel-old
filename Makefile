ISOTARGET := os.iso
FATTARGET := fat.img
EFITARGET := boot/BOOTX64.EFI
KERNEL := kernel/kernel.elf

all: $(ISOTARGET)

$(ISOTARGET): $(FATTARGET)
	mkdir -p iso
	cp $< iso
	xorriso -as mkisofs -R -f -e $< -no-emul-boot -o $@ iso

$(FATTARGET): $(EFITARGET)
	dd if=/dev/zero of=$@ bs=1k count=1440
	mformat -i $@ -f 1440 ::
	mmd -i $@ ::/EFI
	mmd -i $@ ::/EFI/BOOT
	mcopy -i $@ $< ::/EFI/BOOT
	mcopy -i $@ startup.nsh ::
	mcopy -i $@ $(KERNEL) ::

$(EFITARGET):
	cd kernel && make
	cd ..
	cd boot && make
	cd ..

clean:
	rm $(ISOTARGET)
	rm $(FATTARGET)
	rm $(EFITARGET)
	rm *.so