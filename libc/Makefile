SOURCES := $(shell find ./ -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))
TARGET := libc.a
CFLAGS := -ffreestanding -O2 -Wall -Wextra
LDFLAGS := -shared -nostdlib

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	rm -f $(OBJS)
	rm -f $(TARGET)