SOURCES := $(shell find ./ -type f -name '*.c')
ASM := $(shell find ./ -type f -name '*.s')
OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.s, %.o, $(ASM))
LDFLAGS := -T../linker.ld -static -Bsymbolic -nostdlib -L../../libs/libc -l:libc.a
CFLAGS := -ffreestanding -fshort-wchar -mno-red-zone -Wall -Wextra -nostdinc -I../../libs/libc/include -I../../libs/libos/include -fno-stack-protector