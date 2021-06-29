SOURCES := $(shell find ./ -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))
TARGET := libc.a
CFLAGS := -ffreestanding -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -nostdinc -Iinclude

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)