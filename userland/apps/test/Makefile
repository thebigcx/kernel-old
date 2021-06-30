include ../apps.mk

TARGET := test

all: $(TARGET)

$(TARGET): $(OBJS)
	ld -o $(TARGET) $(OBJS) $(LDFLAGS)

%.o: %.c
	gcc $(CFLAGS) -c -o $@ $<

%.o: %.s
	as $< -o $@

.PHONY:
clean:
	@rm -f $(OBJS)
	@rm -f $(TARGET)