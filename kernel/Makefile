SOURCES := $(shell find ./ -type f -name '*.c')
ASM := $(shell find ./ -type f -name '*.s')
OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.s, %.o, $(ASM))
TARGET := kernel.elf
LDFLAGS := -Tkernel.ld -static -Bsymbolic -nostdlib -L../
CFLAGS := -ffreestanding -fshort-wchar -Wall -Wextra -O2 -nostdinc -I../libc/include

kernel: $(TARGET)

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $(TARGET) $(OBJS) -l:libc/libc.a

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.s
	as $< -o $@

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)