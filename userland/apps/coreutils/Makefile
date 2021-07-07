SRCS := $(wildcard *.c)
OBJS := $(patsubst %.c, %.o, $(SRCS))
TARGS := $(patsubst %.c, %, $(SRCS))
LDFLAGS := -T../linker.ld -static -Bsymbolic -nostdlib -L../../libs/libc -l:libc.a
CFLAGS := -ffreestanding -fshort-wchar -mno-red-zone -Wall -Wextra -nostdinc -I../../libs/libc/include -I../../libs/libos/include -fno-stack-protector

all: $(TARGS)

$(TARGS): $(OBJS)
	ld -o $@ $< $(LDFLAGS)

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

.PHONY:
clean:
	@rm -f $(TARGS)
	@rm -f $(OBJS)