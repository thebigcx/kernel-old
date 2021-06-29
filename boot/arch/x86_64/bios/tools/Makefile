SOURCES = $(shell find ./ -type f -name '*.c')
TARGETS = $(patsubst %.c, %, $(SOURCES))

$(TARGETS): $(SOURCES)
	gcc $^ -o $@