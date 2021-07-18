#include <sys/syscall/syscall.h>
#include <sys/console.h>
#include <mem/paging.h>
#include <sched/sched.h>
#include <fs/vfs/vfs.h>
#include <drivers/gfx/fb/fb.h>
#include <drivers/tty/pty/pty.h>
#include <sched/thread.h>

// Some syscalls need the registers
static reg_ctx_t* syscall_regs = NULL;

static fs_fd_t* procgetfd(int fdnum)
{
    proc_t* proc = sched_get_currproc();
    if (fdnum < 0 || fdnum >= proc->file_descs->cnt) // Invalid FD
        return NULL;

    fs_fd_t* fd = (fs_fd_t*)list_get(proc->file_descs, fdnum)->val;
    return fd;
}

static uint64_t sys_open(const char* path, uint32_t flags, uint32_t mode)
{
    proc_t* proc = sched_get_currproc();
    vfs_node_t* node = vfs_resolve_path(path, proc->working_dir);
    if (node == NULL)
    {
        return -1;
    }
    fs_fd_t* fd = vfs_open(node, flags, mode);

    list_push_back(proc->file_descs, fd);

    return proc->file_descs->cnt - 1;
}

// TODO: file descriptor position/offset
static uint64_t sys_read(int fdno, void* ptr, size_t len)
{
    fs_fd_t* fd = procgetfd(fdno);
    if (!fd) return -1;   

    return vfs_read(fd->node, ptr, 0, len);
}

static uint64_t sys_write(int fdno, const void* ptr, size_t len)
{
    fs_fd_t* fd = procgetfd(fdno);
    if (!fd) return -1;

    return vfs_write(fd->node, ptr, 0, len);
}

static uint64_t sys_close(int fdno)
{
    fs_fd_t* fd = procgetfd(fdno);
    if (!fd) return -1;

    vfs_close(fd);
    kfree(fd);
    return 0;
}

static uint64_t sys_mmap(void* addr, size_t len, int prot, int flags, int fdno, size_t off)
{
    proc_t* proc = sched_get_currproc();

    if (fdno)
    {
        fs_fd_t* fd = procgetfd(fdno);
        if (!fd) return -1;

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
    return NULL;
}

static uint64_t sys_ioctl(int fdno, uint64_t cmd, void* argp)
{
    fs_fd_t* fd = procgetfd(fdno);
    if (!fd) return -1;

    return vfs_ioctl(fd->node, cmd, argp);
}

static uint64_t sys_stat(const char* path, vfs_stat_t* status)
{
    return vfs_stat(path, status);
}

static uint64_t sys_fork()
{
    sched_fork(sched_get_currproc(), syscall_regs);
    return 0;
}

uint64_t sys_exec(const char* path, int argc, char** argv)
{
    char* npath = kmalloc(strlen(path) + 1);
    strcpy(npath, path);
    char** nargv = kmalloc(argc * sizeof(char*));
    for (int i = 0; i < argc; i++)
    {
        nargv[i] = kmalloc(strlen(argv[i]) + 1);
        strcpy(nargv[i], argv[i]);
    }
    
    sched_exec(npath, argc, nargv);
    return 0; // Should not return
}

static uint64_t sys_waitpid()
{
    while (1); // TODO
    return 0;
}

static uint64_t sys_exit(int status)
{
    if (status != 0)
    {
        serial_printf("info: proc pid=%d exited with non-zero exit code %d\n", sched_get_currproc()->pid, status);
    }

    sched_kill(sched_get_currproc());
    return 0;
}

static uint64_t sys_sleepns(uint64_t ns)
{
    thread_sleepns(ns);
    return 0;
}

static uint64_t sys_seek(int fdno, int64_t off, int whence)
{
    fs_fd_t* fd = procgetfd(fdno);
    
    switch (whence)
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
static uint64_t sys_openpty(int* master)
{
    pty_t* pty = pty_grant();

    proc_t* proc = sched_get_currproc();
    //list_get(proc->file_descs, 0)->val = vfs_open(pty->slave, 0, 0);
    //list_get(proc->file_descs, 1)->val = vfs_open(pty->slave, 0, 0);

    //list_push_back(proc->file_descs, pty->master);

    *master = proc->file_descs->cnt - 1;
    return 0;
}

static uint64_t sys_threadcreat()
{
    return 0;
}

static uint64_t sys_threadexit()
{
    thread_exit();
    return 0;
}

static uint64_t sys_threadkill(int tid)
{
    proc_t* proc = sched_get_currproc();
    thread_t* found = NULL;

    list_foreach(proc->threads, node)
    {
        thread_t* thread = node->val;
        if (thread->tid == tid)
        {
            found = thread;
            break;
        }
    }

    if (found)
    {
        thread_kill(found);
    }
    return 0;
}

static uint64_t sys_threadjoin()
{
    return 0;
}

static uint64_t sys_sigsend(int pid, int signal)
{
    proc_t* proc = sched_procfrompid(pid);
    signal_send(proc, signal);
    return 0;
}

static uint64_t sys_getpid()
{
    return sched_get_currproc()->pid;
}

static uint64_t (*syscalls[])() =
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
    sys_openpty,
    sys_threadcreat,
    sys_threadexit,
    sys_threadkill,
    sys_threadjoin,
    sys_sigsend,
    sys_getpid
};

void syscall_handler(reg_ctx_t* regs)
{
    uint64_t (*syscall)() = syscalls[regs->rax];
    regs->rax = syscall(regs->rdi, regs->rsi, regs->rdx, regs->rcx, regs->rbx);
}