SOURCES := $(shell find src/ -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))
TARGET := libos.a
CFLAGS := -ffreestanding -O2 -Wall -Wextra -nostdinc -Iinclude -I../kernel/include

all: $(TARGET)

$(TARGET): $(OBJS)
	ar rcs $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)