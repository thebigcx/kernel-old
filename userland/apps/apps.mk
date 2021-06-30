SOURCES := $(shell find ./ -type f -name '*.c')
ASM := $(shell find ./ -type f -name '*.s')
OBJS := $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.s, %.o, $(ASM))
LDFLAGS := -Ttest.ld -static -Bsymbolic -nostdlib -L../../libs/libc -l:libc.a
CFLAGS := -ffreestanding -fshort-wchar -mno-red-zone -Wall -Wextra -nostdinc -fpic -I../../libs/libc/include