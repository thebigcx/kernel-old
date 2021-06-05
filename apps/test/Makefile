SOURCES := $(shell find ./ -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))
TARGET := executable.elf
LDFLAGS := -Ttest.ld -static -Bsymbolic -nostdlib
CFLAGS := -ffreestanding -fshort-wchar -mno-red-zone -Wall -Wextra -nostdinc -fpic

all: $(TARGET)

$(TARGET): $(OBJS)
	ld $(LDFLAGS) -o $(TARGET) $(OBJS)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.s
	as $< -o $@

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)