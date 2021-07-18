SOURCES := shell.c
OBJS := $(patsubst %.c, %.o, $(SOURCES))
TARGET := sh
LDFLAGS := -T../linker.ld -static -Bsymbolic -nostdlib -L../../libs/libc -L../../libs/libos -l:libc.a -l:libos.a
CFLAGS := -ffreestanding -fshort-wchar -mno-red-zone -Wall -Wextra -nostdinc -I../../libs/libc/include -I../../libs/libos/include -fno-stack-protector

all: $(TARGET)

$(TARGET): $(OBJS)
	ld -o $(TARGET) $(OBJS) ../../libs/libc/crt0.o $(LDFLAGS)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

install:
	cp $(TARGET) ../../../base/usr/bin/

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)
