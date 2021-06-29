SOURCES = 
ASM = $(shell find ./ -type f -name '*.S')
OBJS = $(patsubst %.c, %.o, $(SOURCES)) $(patsubst %.S, %.o, $(ASM))
TARGET = boot.img
MBR = mbr.bin
LOADER = loader.bin
LDFLAGS = -Tboot.ld --oformat binary -nostdlib -static -Bsymbolic
CFLAGS = 
ASMFLAGS = -c

all: $(TARGET)

$(TARGET): $(MBR) $(LOADER)
	tools/mkdrv

$(LOADER): loader.o
	ld $(LDFLAGS) -o $(LOADER) $<

$(MBR): mbr.o
	ld $(LDFLAGS) -o $(MBR) $<

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.S
	gcc $(ASMFLAGS) $< -o $@

clean:
	rm $(OBJS)
	rm $(MBR)
	rm $(TARGET)
	rm $(LOADER)