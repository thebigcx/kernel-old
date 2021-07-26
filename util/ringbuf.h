#pragma once

#include <util/types.h>

typedef struct ringbuf
{
	void* buffer;
	size_t size;
	size_t readptr;
	size_t writeptr;

} ringbuf_t;

void ringbuf_create(ringbuf_t* buf, size_t size);
size_t ringbuf_read(ringbuf_t* buf, void* ptr, size_t size);
size_t ringbuf_write(ringbuf_t* buf, const void* ptr, size_t size);
void ringbuf_destroy(ringbuf_t* buf);
