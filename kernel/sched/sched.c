#include <sched/sched.h>
#include <mem/paging.h>
#include <intr/idt.h>
#include <drivers/input/keyboard/ps2kb.h>
#include <drivers/input/mouse/ps2mouse.h>
#include <util/elf.h>
#include <fs/vfs/vfs.h>
#include <util/types.h>
#include <util/stdlib.h>
#include <mem/kheap.h>
#include <sys/system.h>
#include <util/spinlock.h>
#include <intr/apic.h>
#include <time/time.h>
#include <cpu/cpu.h>
#include <drivers/gfx/fb/fb.h>
#include <cpu/smp.h>

void kernel_switch(reg_ctx_t* regs, uint64_t pml4);
void user_switch(reg_ctx_t* regs, uint64_t pml4);

static int scheduler_ready = 0;

extern void kernel_proc();

static list_t* procs;
static lock_t proclist_lock = 0;

int creatpid()
{
    static int pid = 0;
    return pid++;
}

void idle()
{
    while (1);
}

void sched_start()
{
    idt_set_int(IPI_SCHED, schedule); // Software interrupt for schedule (0xfd)

    //idle_proc = sched_mkproc(idle);

    procs = list_create();
    scheduler_ready = 1;
    
    sti();
    for (;;);
}

void schedule(reg_ctx_t* r)
{
    cpu_t* cpu = cpu_getcurr();
    acquire_lock(cpu->lock);

    if (cpu->currthread)
    {
        cpu->currthread->regs = *r;
        // If it hasn't blocked or is sleeping, add it back to the ready list
        if (cpu->currthread->state == THREAD_STATE_RUNNING)
        {
            cpu->currthread->state = THREAD_STATE_READY;
            list_push_back(cpu->run_queue, cpu->currthread);
        }
    }

    if (cpu->run_queue->head != NULL)
    {
        // Remove the next ready to run thread
        cpu->currthread = list_pop_front(cpu->run_queue);
        cpu->currthread->state = THREAD_STATE_RUNNING;

        if (cpu->currthread->tid == 0)
        {
            proc_t* curr = sched_get_currproc();
            if (curr->sigstack->cnt > 0)
            {
                signal_t* sig = list_pop_front(curr->sigstack);
                signal_handle(curr, sig);
            }
        }

        release_lock(cpu->lock);

        if (cpu->currthread->regs.cs == KERNEL_CS)
            kernel_switch(&(cpu->currthread->regs), (uint64_t)cpu->currthread->parent->addr_space->pml4_phys);
        else
            user_switch(&(cpu->currthread->regs), (uint64_t)cpu->currthread->parent->addr_space->pml4_phys);
    }

    release_lock(cpu->lock);
    for (;;);
}

proc_t* sched_mkproc(void* entry)
{
    proc_t* proc     = kmalloc(sizeof(proc_t));
    proc->addr_space = page_mk_map();
    proc->pid        = creatpid();
    proc->file_descs = list_create();
    proc->threads    = list_create();
    proc->children   = list_create();
    proc->nexttid    = 1;
    proc->parent     = NULL;
    proc->sigstack   = list_create();

    strcpy(proc->name, "unknown");

    thread_t* thread = kmalloc(sizeof(thread_t));
    list_push_back(proc->threads, thread);

    void* stack = page_kernel_alloc4k(16);
    
    for (uint32_t i = 0; i < 16; i++)
    {
        page_kernel_map_memory(stack + i * PAGE_SIZE_4K, pmm_request(), 1);
    }

    thread->state = THREAD_STATE_READY;
    thread->parent = proc;
    thread->sleepexp = 0;
    thread->tid = 0; // Primary thread
    
    reg_ctx_t* regs = &thread->regs;

    memset(regs, 0, sizeof(reg_ctx_t));
    regs->rip = (uint64_t)entry;
    regs->rflags = RFLAG_INTR | 0x2; // Interrupts, reserved
    regs->cs = KERNEL_CS;
    regs->ss = KERNEL_SS;
    regs->rbp = (uint64_t)stack + 0x10000;
    regs->rsp = (uint64_t)stack + 0x10000;

    return proc;
}

void sched_tick(reg_ctx_t* r)
{
    if (!scheduler_ready) return;

    lapic_send_ipi(0, ICR_ALL_EX_SELF, ICR_FIXED, IPI_SCHED);
    schedule(r);
}

void sched_yield()
{
    asm volatile ("int $0xfd"); // Schedule IPI to self
}

void sched_spawn(proc_t* proc, proc_t* parent)
{
    acquire_lock(proclist_lock);
    //list_push_back(procs, proc);
    release_lock(proclist_lock);

    thread_spawn(proc->threads->head->val);
}

// TODO: proc_t struct cleanup
void sched_kill(proc_t* proc)
{
    // Kill every thread in the process
    list_foreach(proc->threads, node)
    {
        thread_t* thread = node->val;
        thread_kill(thread);
    }
    if (proc == sched_get_currproc())
    {
        sched_yield();
    }
}

void sched_proc_destroy(proc_t* proc)
{
    uint32_t files = proc->file_descs->cnt;
    for (uint32_t i = 0; i < files; i++)
    {
        fs_fd_t* fd = list_pop_front(proc->file_descs);
        vfs_close(fd);
    }

    page_destroy_map(proc->addr_space);

    kfree(proc);
}

void sched_fork(proc_t* proc, reg_ctx_t* regs)
{
    proc_t* nproc = kmalloc(sizeof(proc_t));
    nproc->addr_space = page_clone_map(proc->addr_space);
    nproc->pid        = creatpid();
    nproc->file_descs = list_create();
    nproc->threads    = list_create();
    nproc->children   = list_create();
    nproc->nexttid    = 1;
    nproc->parent     = NULL;
    nproc->sigstack   = list_create();

    nproc->name = strdup("unknown");

    thread_t* thread = kmalloc(sizeof(thread_t));
    list_push_back(nproc->threads, thread);

    list_foreach(proc->file_descs, node)
    {
        fs_fd_t* fd = node->val;
        list_push_back(nproc->file_descs, vfs_open(fd->node, fd->flags, fd->mode));
    }

    space_alloc_region_at(0x20000, 0x4000, nproc->addr_space);
    for (uint32_t i = 0; i < 0x4000; i += PAGE_SIZE_4K)
    {
        page_map_memory(0x20000 + i, pmm_request(), 1, nproc->addr_space);
    }

    thread->parent = nproc;
    thread->state = THREAD_STATE_READY;
    thread->sleepexp = 0;
    thread->tid = 0; // Primary thread

    memcpy(&thread->regs, &((thread_t*)proc->threads->head->val)->regs, sizeof(reg_ctx_t));
    
    
    /*proc_t* new = kmalloc(sizeof(proc_t));
    new->addr_space = page_clone_map(proc->addr_space);
    new->pid = creatpid();
    new->sleep_exp = 0;
    new->file_descs = list_create();
    new->state = proc->state;
    strcpy(new->name, proc->name);
    
    new->regs = *regs;

    if (proc->working_dir)
        strcpy(new->working_dir, proc->working_dir);

    list_foreach(proc->file_descs, node)
    {
        fs_fd_t* fd = node->val;
        fs_fd_t* newfd = vfs_open(fd->node, 0, 0); // vfs_node_t does not need to be copied
        list_push_back(new->file_descs, newfd);
    }

    // Child processes get a new stack
    space_alloc_region_at(0x20000, 0x4000, proc->addr_space);
    sched_spawn(new, proc);*/
}

proc_t* sched_get_currproc()
{
    //return currproc;
    cpu_t* cpu = cpu_getcurr();
    
    if (cpu->currthread)
        return cpu->currthread->parent;

    return NULL;
}

bool check_elf_hdr(elf64_hdr_t* hdr)
{
    if (hdr->ident[0] != ELFMAG0 ||
        hdr->ident[1] != ELFMAG1 ||
        hdr->ident[2] != ELFMAG2 ||
        hdr->ident[3] != ELFMAG3)
    {
        
        return false;
    }

    if (hdr->ident[EI_CLASS] != ELFCLASS64) return false;

    return true;
}

static uint64_t loadelf(void* data, proc_t* proc)
{
    elf_inf_t inf;
    elf_load(data, proc, &inf);
    return inf.entry;
}

// Prepare the stack of a process, by pushing the necessary variables (argc, argv, env, envp)
static void prepstack(proc_t* proc, const char* file, int argc, char** argv, int envp, char** env)
{
    thread_t* thread = (thread_t*)proc->threads->head->val;

    uint64_t stack = thread->regs.rsp;

    uint64_t* tmp_argv = kmalloc((argc + 1) * sizeof(uint64_t)); // First arg is exec path

    // Push 'args' onto the stack (load proc's virtual address space)
    uint64_t cr3;
    asm volatile ("mov %%cr3, %0" : "=r"(cr3));

    cli();
    asm volatile ("mov %%rax, %%cr3" :: "a"(proc->addr_space->pml4_phys));

    stack -= strlen(file) + 1;
    strcpy((char*)stack, file);
    tmp_argv[0] = stack;

    for (int i = 0; i < argc; i++)
    {
        stack -= strlen(argv[i]) + 1;
        strcpy(stack, argv[i]);
        tmp_argv[i + 1] = stack;
        
    }

    for (int i = argc; i >= 0; i--)
    {
        stack -= sizeof(uint64_t);
        *((uint64_t*)stack) = tmp_argv[i];
    }

    asm volatile ("mov %%rax, %%cr3" :: "a"(cr3));
    sti();

    thread->regs.rsp = stack;
    thread->regs.rsi = stack;
    
    kfree(tmp_argv);
}

proc_t* sched_mkelfproc(const char* path, void* data, int argc, char** argv, int envp, char** env)
{
    proc_t* proc     = kmalloc(sizeof(proc_t));
    proc->pid        = creatpid();
    proc->addr_space = page_mk_map(); // Creates a virtual address space with kernel mapped
    proc->file_descs = list_create();
    proc->threads    = list_create();
    proc->children   = list_create();
    proc->nexttid    = 1;
    proc->parent     = NULL;
    proc->sigstack   = list_create();

    strcpy(proc->name, "unknown");

    thread_t* thread = kmalloc(sizeof(thread_t));
    list_push_back(proc->threads, thread);

    // TODO: These are temporary - later will be hooked up to PTYs
    vfs_node_t* console = vfs_resolve_path("/dev/console", NULL);

    fs_fd_t* stdin  = vfs_open(console, 0, 0);
    fs_fd_t* stdout = vfs_open(console, 0, 0);

    list_push_back(proc->file_descs, stdin);
    list_push_back(proc->file_descs, stdout);

    // TODO: it should be this
    /*uint64_t stacktop = KERNEL_VIRTUAL_ADDR;
    uint64_t stackbot = KERNEL_VIRTUAL_ADDR - 0x4000;

    space_alloc_region_at(stackbot, 0x4000, proc->addr_space);
    for (uint64_t i = stackbot; i < stacktop; i += PAGE_SIZE_4K)
    {
        page_map_memory(i, pmm_request(), 1, proc->addr_space);
    }*/

    space_alloc_region_at(0x20000, 0x4000, proc->addr_space);
    for (uint32_t i = 0; i < 0x4000; i += PAGE_SIZE_4K)
    {
        page_map_memory(0x20000 + i, pmm_request(), 1, proc->addr_space);
    }

    thread->parent = proc;
    thread->state = THREAD_STATE_READY;
    thread->sleepexp = 0;
    thread->tid = 0; // Primary thread

    reg_ctx_t* regs = &thread->regs;
    
    memset(regs, 0, sizeof(reg_ctx_t));
    regs->rip = loadelf(data, proc);
    regs->rflags = RFLAG_INTR | 0x2; // Interrupts, reserved
    regs->cs = USER_CS;
    regs->ss = USER_SS;
    regs->rbp = (uint64_t)0x24000;
    regs->rsp = (uint64_t)0x24000;

    prepstack(proc, path, argc, argv, 0, NULL);

    return proc;
}

// Cheap way of implementing exec() - spawning a new process and killing the old one
void sched_exec(const char* path, int argc, char** argv)
{
    vfs_node_t* node = vfs_resolve_path(path, NULL);
    uint8_t* buffer = kmalloc(node->size);
    vfs_read(node, buffer, 0, node->size);

    proc_t* proc = sched_mkelfproc(path, buffer, argc, argv, 0, NULL);

    kfree(buffer);

    sched_spawn(proc, NULL);
    sched_kill(sched_get_currproc());
    sched_yield();
}

proc_t* sched_procfrompid(int pid)
{
    // TEMP
    return sched_get_currproc();
    acquire_lock(proclist_lock);

    list_foreach(procs, node)
    {
        proc_t* proc = node->val;
        if (proc->pid == pid)
        {
            release_lock(proclist_lock);
            return proc;
        }
    }

    release_lock(proclist_lock);
    return NULL;
}

sem_t* sem_create(uint32_t max_cnt)
{
    sem_t* sem = kmalloc(sizeof(sem_t));
    sem->curr_cnt = 0;
    sem->max_cnt = max_cnt;
    sem->waitlst = list_create();
    return sem;
}

void sem_acquire(sem_t* sem)
{
    if (sem->curr_cnt < sem->max_cnt)
    {
        sem->curr_cnt++;
    }
    else
    {
        //list_push_back(sem->waitlst, currproc);
        //sched_block(PROC_STATE_WAIT_LOCK);
        cpu_t* cpu = cpu_getcurr();
        list_push_back(sem->waitlst, cpu->currthread);
        thread_block(THREAD_STATE_WAIT_LOCK);
    }
}

void sem_release(sem_t* sem)
{
    if (sem->waitlst->head != NULL)
        thread_unblock(list_pop_front(sem->waitlst));
    else
        sem->curr_cnt--;
}

mutex_t* mutex_create()
{
    mutex_t* mutex = kmalloc(sizeof(mutex_t));
    mutex->waitlst = list_create();
    mutex->proc = 0;
    return mutex;
}

void mutex_acquire(mutex_t* mutex)
{
    if (!mutex->proc)
    {
        mutex->proc = 1;
    }
    else
    {
        //list_push_back(mutex->waitlst, currproc);
        //sched_block(PROC_STATE_WAIT_LOCK);
        cpu_t* cpu = cpu_getcurr();
        list_push_back(mutex->waitlst, cpu->currthread);
        thread_block(THREAD_STATE_WAIT_LOCK);
    }
}

void mutex_release(mutex_t* mutex)
{
    if (mutex->waitlst->head != NULL)
        thread_unblock(list_pop_front(mutex->waitlst));
    else
        mutex->proc = 0;
}

#define SIGACT_TERM 0
#define SIGACT_IGN  1
#define SIGACT_STOP 2

// Default signal actions
int sigacts[SIGCNT] =
{
    [SIGINT] = SIGACT_TERM,
    [SIGSEG] = SIGACT_STOP,
    [SIGABRT] = SIGACT_STOP
};

const char* sigstrs[SIGCNT] =
{
    [SIGINT] = "SIGINT",
    [SIGSEG] = "SIGSEG",
    [SIGABRT] = "SIGABRT"
};

void signal_handle(proc_t* proc, signal_t* sig)
{
    if (proc->signals[sig->signo])
    {
        thread_t* thread = proc->threads->head;
        thread->regs.rip = sig->handler;
    }
    else
    {
        switch (sigacts[sig->signo])
        {
            case SIGACT_IGN:
            {
                break;
            }
            // For now, these actions are the same
            case SIGACT_TERM:
            case SIGACT_STOP:
            {
                serial_printf("info: proc pid=%d received signal %d, %s\n", sched_get_currproc()->pid, sig->signo, sigstrs[sig->signo]);
                sched_kill(sched_get_currproc());
                sched_yield();
                break;
            }
        }
    }
}
