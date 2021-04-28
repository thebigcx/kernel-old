SOURCES := $(shell find ./ -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))
TARGET := kernel.elf
LDFLAGS := -Tkernel.ld -static -Bsymbolic -nostdlib -L../
CFLAGS := -ffreestanding -fshort-wchar -Wall -Wextra -O2 -nostdinc -I../libc/include

kernel: $(TARGET)

$(TARGET): $(OBJS)
	pwd
	ld $(LDFLAGS) -o $(TARGET) $(OBJS) -l:libc/libc.a

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)