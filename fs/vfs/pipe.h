#pragma once

#include <fs/vfs/vfs.h>
#include <util/ringbuf.h>

typedef struct pipe
{
	vfs_node_t* producer;
	vfs_node_t* consumer;	
	ringbuf_t* buf;

} pipe_t;

void pipe_creat(vfs_node_t** nodes);
size_t pipe_read(vfs_node_t* node, void* ptr, size_t off, size_t size);
size_t pipe_write(vfs_node_t* node, const void* ptr, size_t off, size_t size);

