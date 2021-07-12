#include <sys/syscall/syscall.h>
#include <sys/console.h>
#include <mem/paging.h>
#include <sched/sched.h>
#include <drivers/fs/vfs/vfs.h>
#include <drivers/gfx/fb/fb.h>
#include <drivers/tty/pty/pty.h>

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
    proc_t* proc = sched_get_currproc();
    fs_fd_t* fd = (fs_fd_t*)list_get(proc->file_descs, regs->rdi)->val;
    vfs_close(fd);
    kfree(fd);
    return 0;
}

// TODO: fix bug where memory cannot be mapped from a syscall
uint64_t sys_mmap(reg_ctx_t* regs)
{
    void* addr = regs->rdi;
    size_t len = regs->rsi;
    int prot = regs->rdx;
    int flags = regs->r10;
    int fdno = regs->r8;
    size_t off = regs->r9;

    proc_t* proc = sched_get_currproc();

    if (fdno)
    {
        fs_fd_t* fd = procgetfd(fdno);
        vfs_node_t* node = fd->node;

        if (node->mmap)
            return node->mmap(node, proc, addr, len, prot, flags, off);
    }
    else
    {
        void* mem = space_alloc_region(len, proc->addr_space);
        for (uint32_t i = 0; i < len / PAGE_SIZE_4K; i++)
        {
            page_map_memory(mem + i * PAGE_SIZE_4K, pmm_request(), 1, proc->addr_space);
        }

        return mem;
    }
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
    return vfs_stat(regs->rdi, regs->rsi);
}

uint64_t sys_fork(reg_ctx_t* regs)
{
    sched_fork(sched_get_currproc(), regs);
}

uint64_t sys_exec(reg_ctx_t* regs)
{
    char* path = kmalloc(strlen(regs->rdi) + 1);
    strcpy(path, regs->rdi);
    int argc = regs->rsi;
    char** argv = kmalloc(argc * sizeof(char*));
    for (int i = 0; i < argc; i++)
    {
        char* arg = ((char**)regs->rdx)[i];
        argv[i] = kmalloc(strlen(arg) + 1);
        strcpy(argv[i], arg);
    }
    /*char* str = kmalloc(strlen(regs->rdi) + 1); // mkelfproc switches page maps, so we must copy the user args
    strcpy(str, regs->rdi);
    proc_t* new = mkelfproc(str, regs->rsi, regs->rdx, 0, NULL); // TODO: args
    kfree(str);
    sched_spawn(new);*/
    //sched_exec(path, argc, argv);
    sched_exec(path, argc, argv);
    return 0; // Should not return
}

uint64_t sys_waitpid(reg_ctx_t* regs)
{
    while (1); // TODO
}

uint64_t sys_exit(reg_ctx_t* regs)
{
    //sched_terminate();
}

uint64_t sys_sleepns(reg_ctx_t* regs)
{
    //sched_sleepns(regs->rdi);
}

uint64_t sys_seek(reg_ctx_t* regs)
{
    fs_fd_t* fd = procgetfd(regs->rdi);
    int64_t off = regs->rsi;
    
    switch (regs->rdx)
    {
        case SEEK_SET:
            fd->pos = off;
            break;
        case SEEK_CUR:
            fd->pos += off;
            break;
        case SEEK_END:
            fd->pos = fd->node->size + off;
            break;
    }
    return 0;
}

// TODO: delete resources properly and add stderr
uint64_t sys_openpty(reg_ctx_t* regs)
{
    pty_t* pty = pty_grant();

    proc_t* proc = sched_get_currproc();
    //list_get(proc->file_descs, 0)->val = vfs_open(pty->slave, 0, 0);
    //list_get(proc->file_descs, 1)->val = vfs_open(pty->slave, 0, 0);

    //list_push_back(proc->file_descs, pty->master);

    *((int*)regs->rdi) = proc->file_descs->cnt - 1;
    return 0;
}

syscall_t syscalls[SYSCALL_CNT] =
{
    sys_read,
    sys_write,
    sys_open,
    sys_close,
    sys_mmap,
    sys_ioctl,
    sys_stat,
    sys_fork,
    sys_exec,
    sys_waitpid,
    sys_exit,
    sys_sleepns,
    sys_seek,
    sys_openpty
};

void syscall_handler(reg_ctx_t* regs)
{
    regs->rax = syscalls[regs->rax](regs);
}