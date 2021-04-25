SOURCES := $(shell find ./ -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))
TARGET := kernel.elf
LDFLAGS := -Tkernel.ld -static -Bsymbolic -nostdlib -L ../libc -l:libc.a
CFLAGS := -ffreestanding -fshort-wchar -Wall -Wextra -O2

kernel: $(TARGET)

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

.PHONY:
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)