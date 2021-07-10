SOURCES := $(shell find ./ -type f -name '*.c')
OBJS := $(patsubst %.c, %.o, $(SOURCES))
LDFLAGS := -shared -Bsymbolic -nostdlib -L../libs/libc -l:libc.a -e _start
CFLAGS := -ffreestanding -fshort-wchar -mno-red-zone -Wall -Wextra -nostdinc -I../libs/libc/include -I../libs/libos/include -fno-stack-protector

TARGET := ld.so

all: $(TARGET)

$(TARGET): $(OBJS)
	ld -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

install:
	cp $(TARGET) ../../base/usr/lib/

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)