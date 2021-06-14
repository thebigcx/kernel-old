SOURCES := $(shell find ./kernel ./drivers -type f -name '*.c')
ASM := $(shell find ./ -type f -name '*.s')
OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.s, %.o, $(ASM))
TARGET := kernel.elf
LDFLAGS := -Tkernel.ld -static -Bsymbolic -nostdlib
CFLAGS := -ffreestanding -fshort-wchar -fno-stack-protector -mno-red-zone -Wall -Wextra -Wno-unused-variable -Wno-unused-parameter -O2 -nostdinc -Ikernel -I.

kernel: $(TARGET)

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $(TARGET) $(OBJS)

kernel/%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

drivers/%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

kernel/%.o: %.s
	as $< -o $@

drivers/%.o: %.s
	as $< -o $@

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)