SOURCES := $(shell find ./ -type f -name '*.c')
ASM := $(shell find ./ -type f -name '*.s')
OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.s, %.o, $(ASM))
TARGET := kernel.elf
LDFLAGS := -Tkernel.ld -static -Bsymbolic -nostdlib -L../
CFLAGS := -ffreestanding -fshort-wchar -mno-red-zone -Wall -Wextra -O2 -nostdinc -I../libc/include -Iinclude

kernel: $(TARGET)

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $(TARGET) $(OBJS) -l:libc/libc.a

kernel/gdt/irq.o: kernel/gdt/irq.c
	gcc -Iinclude -mno-red-zone -mgeneral-regs-only -ffreestanding -c -o $@ $<

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.s
	as $< -o $@

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)