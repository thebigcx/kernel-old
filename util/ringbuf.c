#include <util/ringbuf.h>

void ringbuf_create(ringbuf_t* buf, size_t size)
{
	buf->buffer = kmalloc(size);
	buf->size = size;
	buf->readptr = 0;
	buf->writeptr = 0;
}

size_t ringbuf_read(ringbuf_t* buf, void* ptr, size_t size)
{
		
}

size_t ringbuf_write(ringbuf_t* buf, const void* ptr, size_t size)
{

}

void ringbuf_destroy(ringbuf_t* buf)
{
	kfree(buf->buffer);
}
