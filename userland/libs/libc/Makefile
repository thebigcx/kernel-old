SOURCES := $(shell find ./ -type f -name '*.c')
ASM := $(shell find ./ -type f -name '*.S')
OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.S, %.o, $(ASM))
TARGET := libc.a
CFLAGS := -ffreestanding -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -nostdinc -Iinclude

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

%.o: %.S
	gcc $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)