#include <sys/syscall/syscall.h>
#include <sys/console.h>
#include <mem/paging.h>
#include <sched/sched.h>
#include <fs/vfs/vfs.h>
#include <drivers/gfx/fb/fb.h>
#include <drivers/tty/pty/pty.h>
#include <sched/thread.h>
#include <arch/x86_64/syscall.h>
#include <time/time.h>

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

uint64_t sys_execve(const char* path, char** argv, char** envp)
{
	int argc = 0;
	while (argv[argc] != NULL) argc++;

    char* npath = kmalloc(strlen(path) + 1);
    strcpy(npath, path);
    char** nargv = kmalloc(argc * sizeof(char*));
    for (int i = 0; i < argc; i++)
    {
        nargv[i] = kmalloc(strlen(argv[i]) + 1);
        strcpy(nargv[i], argv[i]);
    }
    
    sched_exec(npath, argc, nargv);
    serial_printf("EXEC");
    return 0; // Should not return
}

static uint64_t sys_waitid()
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

static uint64_t sys_nanosleep(timespec_t* req, timespec_t* rem)
{
    //thread_sleepns(ns);
    return 0;
}

static uint64_t sys_lseek(unsigned int fdno, int64_t off, int whence)
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

static uint64_t sys_create_thread(uint64_t* tid, void* entry, void* arg)
{
	thread_t* thread = thread_creat(sched_get_currproc(), entry, 0);
	thread_spawn(thread);
	*tid = thread->tid;	
    return 0;
}

static uint64_t sys_exit_thread()
{
    thread_exit();
    return 0;
}

static uint64_t sys_kill_thread(int tid)
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

static uint64_t sys_join_thread()
{
    return 0;
}

static uint64_t sys_kill(int pid, int signal)
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
    [SYS_READ] = sys_read,
    [SYS_WRITE] = sys_write,
    [SYS_OPEN] = sys_open,
    [SYS_CLOSE] = sys_close,
    [SYS_MMAP] = sys_mmap,
    [SYS_IOCTL] = sys_ioctl,
    [SYS_STAT] = sys_stat,
    [SYS_FORK] = sys_fork,
    [SYS_EXECVE] = sys_execve,
    [SYS_WAITID] = sys_waitid,
    [SYS_EXIT] = sys_exit,
    [SYS_NANOSLEEP] = sys_nanosleep,
    [SYS_LSEEK] = sys_lseek,
    [SYS_CREATETHREAD] = sys_create_thread,
    [SYS_EXITTHREAD] = sys_exit_thread,
    [SYS_KILLTHREAD] = sys_kill_thread,
    [SYS_JOINTHREAD] = sys_join_thread,
    [SYS_KILL] = sys_kill,
    [SYS_GETPID] = sys_getpid
};

void syscall_handler(reg_ctx_t* regs)
{
	uint64_t (*syscall)() = syscalls[arch_syscall_num(regs)];
	arch_syscall_ret(regs, syscall(ARCH_SCARG0(regs),
								   ARCH_SCARG1(regs),
								   ARCH_SCARG2(regs),
								   ARCH_SCARG3(regs),
								   ARCH_SCARG4(regs)));
}
