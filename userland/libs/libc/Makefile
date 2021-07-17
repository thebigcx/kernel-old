SOURCES := internal/init.c	\
		   malloc/malloc.c	\
		   math/math.c		\
		   stdio/file.c		\
		   stdio/printf.c	\
		   stdio/stdio.c	\
		   stdlib/abs.c		\
		   stdlib/convert.c	\
		   stdlib/rand.c	\
		   string/string.c	\
		   unistd/file.c	\
		   unistd/proc.c	\

OBJS := $(patsubst %.c, %.o, $(SOURCES))
AR := libc.a
SO := libc.so
CRT0 := crt0.o
CFLAGS := -ffreestanding -fno-stack-protector -O2 -Wall -Wextra -Wno-unused-parameter -Wno-unused-variable -nostdinc -Iinclude -I../libos/include

all: $(AR) $(SO) $(CRT0)

$(AR): $(OBJS)
	ar rcs $@ $^

$(SO): $(OBJS)
	ld -shared -nostdlib -Bsymbolic -o $@ $^

%.o: %.c
	gcc $(CFLAGS) -c $< -o $@

$(CRT0): crt0.c
	gcc $(CFLAGS) -c $< -o $@

install:
	cp $(AR) ../../../base/usr/lib/
	cp $(SO) ../../../base/usr/lib/
	cp $(CRT0) ../../../base/usr/lib/

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)
	@rm -f $(CRT0)