SOURCES := $(shell find ./ -type f -name '*.c')
ASM := $(shell find ./ -type f -name '*.S')
OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.S, %.o, $(ASM))
AR := libc.a
SO := libc.so
CFLAGS := -ffreestanding -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -nostdinc -Iinclude -I../libos/include

all: $(AR) $(SO)

$(AR): $(OBJS)
	ar rcs $@ $^

$(SO): $(OBJS)
	ld -shared -nostdlib -Bsymbolic -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

%.o: %.S
	gcc $(CFLAGS) -c $< -o $@

install:
	cp $(AR) ../../../base/usr/lib/
	cp $(SO) ../../../base/usr/lib/

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)