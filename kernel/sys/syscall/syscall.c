#include <sys/syscall/syscall.h>
#include <sys/console.h>
#include <mem/paging.h>
#include <sched/sched.h>
#include <drivers/fs/vfs/vfs.h>

#define SYSCALL_CNT 256

static fs_fd_t* procgetfd(int fdnum)
{
    proc_t* proc = sched_get_currproc();
    fs_fd_t* fd = (fs_fd_t*)list_get(proc->file_descs, fdnum)->val;
    return fd;
}

uint64_t sys_open(reg_ctx_t* regs)
{
    char* path = regs->rdi;

    proc_t* proc = sched_get_currproc();
    vfs_node_t* node = vfs_resolve_path(path, proc->working_dir);
    fs_fd_t* fd = vfs_open(node, regs->rsi, regs->rdx);

    list_push_back(proc->file_descs, fd);

    return proc->file_descs->cnt - 1;
}

// TODO: file descriptor position/offset
uint64_t sys_read(reg_ctx_t* regs)
{
    fs_fd_t* fd = procgetfd(regs->rdi);
    uint8_t* buf = regs->rsi;
    size_t size = regs->rdx;

    return vfs_read(fd->node, buf, 0, size);
}

uint64_t sys_write(reg_ctx_t* regs)
{
    fs_fd_t* fd = procgetfd(regs->rdi);
    uint8_t* buf = regs->rsi;
    size_t size = regs->rdx;

    return vfs_write(fd->node, buf, 0, size);
}

uint64_t sys_close(reg_ctx_t* regs)
{
    console_write("Syscall close!", 255, 255, 255);
}

uint64_t sys_mmap(reg_ctx_t* regs)
{
    void* addr = regs->rdi;
    size_t* len = regs->rsi;
    int prot = regs->rdx;
    int flags = regs->r10;
    int fdno = regs->r8;
    size_t off = regs->r9;

    proc_t* proc = sched_get_currproc();
    fs_fd_t* fd = procgetfd(fdno);
    vfs_node_t* node = fd->node;

    if (node->mmap)
        return node->mmap(node, proc, addr, len, prot, flags, off);
}

uint64_t sys_ioctl(reg_ctx_t* regs)
{
    fs_fd_t* fd = procgetfd(regs->rdi);
    uint64_t cmd = regs->rsi;
    void* argp = regs->rdx;

    return vfs_ioctl(fd->node, cmd, argp);
}

uint64_t sys_stat(reg_ctx_t* regs)
{
    return vfs_stat(regs->rdi);
}

syscall_t syscalls[SYSCALL_CNT] =
{
    sys_read,
    sys_write,
    sys_open,
    sys_close,
    sys_mmap,
    sys_ioctl,
    sys_stat
};

void syscall_handler(reg_ctx_t* regs)
{
    regs->rax = syscalls[regs->rax](regs);
}