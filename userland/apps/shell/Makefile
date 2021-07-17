include ../apps.mk

TARGET := sh

all: $(TARGET)

$(TARGET): $(OBJS)
	ld -o $(TARGET) $(OBJS) ../../libs/libc/crt0.o $(LDFLAGS)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.s
	as $< -o $@

install:
	cp $(TARGET) ../../../base/usr/bin/

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)