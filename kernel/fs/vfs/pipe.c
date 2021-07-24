#include <fs/vfs/pipe.h>

void pipe_creat(vfs_node_t** nodes)
{
	nodes[0] = kmalloc(sizeof(vfs_node_t));
	nodes[1] = kmalloc(sizeof(vfs_node_t));		
}

size_t pipe_read(vfs_node_t* node, void* ptr, size_t off, size_t size)
{
	pipe_t* pipe = node->device;
}

size_t pipe_write(vfs_node_t* node, const void* ptr, size_t off, size_t size)
{
	pipe_t* pipe = node->device;
}
