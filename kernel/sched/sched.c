#include <sched/sched.h>
#include <mem/paging.h>
#include <intr/idt.h>
#include <drivers/input/keyboard/ps2_keyboard.h>
#include <drivers/input/mouse/ps2_mouse.h>
#include <util/elf.h>
#include <drivers/fs/vfs/vfs.h>
#include <util/types.h>
#include <util/stdlib.h>
#include <mem/kheap.h>
#include <sys/system.h>
#include <sched/spinlock.h>
#include <intr/apic.h>
#include <time/time.h>

void ctx_switch(reg_ctx_t* regs, uint64_t pml4);

proc_t* ready_lst_start;
proc_t* ready_lst_end;
lock_t ready_lst_lock = 0;
proc_t* last_proc;

proc_t* idle_proc;

proc_t* sleep_tsk_lst;

int scheduler_ready = 0;

void idle()
{
    while (1);
    //hlt();
}

void sched_init()
{
    idt_set_int(IPI_SCHED, schedule); // Software interrupt for schedule (0xfd)

    idle_proc = mk_proc(idle);

    scheduler_ready = 1;
    
    sti();
    for (;;);
}

void schedule(reg_ctx_t* r)
{
    proc_t* tsk = ready_lst_start;

    //if (!tsk) tsk = idle_proc;

    if (tsk != NULL)
    {
        last_proc->regs = *r;
        if (last_proc->state == PROC_STATE_RUNNING)
            last_proc->state = PROC_STATE_READY;

        ready_lst_start = tsk->next;
        last_proc = tsk;
        tsk->state = PROC_STATE_RUNNING;

        ctx_switch(&(tsk->regs), (uint64_t)tsk->addr_space->pml4_phys);
    }
    //else if (last_proc->state == PROC_STATE_RUNNING)
    {
        // Let it continue running
    }
    /*else
    {
        proc_t* tsk = last_proc;
        last_proc = NULL;
    
        do
        {
            hlt();
        } while (ready_lst_start == NULL); // Wait for task

        last_proc = tsk;
        tsk = ready_lst_start;
        if (tsk != last_proc)
        {
            ctx_switch(&(tsk->regs), (uint64_t)tsk->addr_space);
        }
    }*/
}

proc_t* mk_proc(void* entry)
{
    proc_t* proc = kmalloc(sizeof(proc_t));
    proc->state = PROC_STATE_READY;
    proc->addr_space = page_mk_map();
    proc->pid = 0;
    proc->next = NULL;
    proc->sleep_exp = 0;
    proc->file_descs = list_create();

    void* stack = page_kernel_alloc4k(16);
    for (uint32_t i = 0; i < 16; i++)
    {
        page_kernel_map_memory(stack + i * PAGE_SIZE_4K, pmm_request(), 1);
    }

    memset(&(proc->regs), 0, sizeof(reg_ctx_t));
    proc->regs.rip = (uint64_t)entry;
    proc->regs.rflags = 0x202;
    proc->regs.cs = KERNEL_CS;
    proc->regs.ss = KERNEL_SS;
    proc->regs.rbp = (uint64_t)stack + 0x16000;
    proc->regs.rsp = (uint64_t)stack + 0x16000;

    return proc;
}

void sched_tick(reg_ctx_t* r)
{
    if (!scheduler_ready) return;

    /*cli();

    proc_t* next = sleep_tsk_lst;
    proc_t* this;
    sleep_tsk_lst = NULL;

    while (next != NULL)
    {
        this = next;
        next = this->next;

        if (this->sleep_exp <= pit_uptime() * 1000)
        {
            sched_unblock(this);
        }
        else
        {
            this->next = sleep_tsk_lst;
            sleep_tsk_lst = this;
        }
    }

    sti();*/

    schedule(r);
}

void sched_spawn_proc(proc_t* proc)
{
    //acquire_lock(ready_lst_lock);

    if (!ready_lst_start || !ready_lst_end) // First process
    {
        ready_lst_start = proc;
        ready_lst_end = proc;
        //proc->next = proc;
        return;
    }

    proc_t* last = ready_lst_end;
    last->next = proc;
    ready_lst_end = proc;
    //proc->next = ready_lst_start;

    //release_lock(ready_lst_lock);
}

void sched_kill_proc()
{
    /*sched_lock();

    last_proc->next = kill_tsk_lst;
    kill_tsk_lst = last_proc;

    sched_unlock();

    sched_block(PROC_STATE_KILLED);

    sched_unblock();*/
}

void sched_block(uint32_t state)
{
    cli();

    last_proc->state = state;
    lapic_send_ipi(0, ICR_ALL_EX_SELF, ICR_FIXED, IPI_SCHED);

    sti();
}

void sched_unblock(proc_t* proc)
{
    cli();

    if (ready_lst_start)
    {
        // Need to wait (other processes running)
        ready_lst_end->next = proc;
        ready_lst_end = proc;
        proc->state = PROC_STATE_READY;
    }
    else
    {
        ready_lst_start = proc;
        ready_lst_end = proc;
    }

    sti();
}

void sched_fork(proc_t* proc)
{
    proc_t* new = kmalloc(sizeof(proc_t));
    new->addr_space = page_clone_map(proc->addr_space);
}

proc_t* sched_get_currproc()
{
    return last_proc;
}

static void nano_sleep_until(uint64_t t)
{
    cli();

    if (t < pit_uptime() * 1000)
    {
        sti();
        return;
    }

    last_proc->sleep_exp = t;
    last_proc->next = sleep_tsk_lst;
    sleep_tsk_lst = last_proc;

    sti();

    sched_block(PROC_STATE_SLEEP);
}

void nano_sleep(uint64_t ns)
{
    nano_sleep_until(pit_uptime() * 1000 + ns);
}

void sleep(uint64_t s)
{
    nano_sleep(s * 1000000000);
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

void* loadelf(uint8_t* elf_dat, proc_t* proc)
{
    elf64_hdr_t hdr;

    memcpy(&hdr, elf_dat, sizeof(elf64_hdr_t));

    if (!check_elf_hdr(&hdr))
        return NULL;

    for (uint16_t i = 0; i < hdr.ph_num; i++)
    {
        elf64_phdr_t phdr;
        memcpy(&phdr, elf_dat + hdr.ph_off + hdr.ph_ent_size * i, sizeof(elf64_phdr_t));

        if (phdr.type == PT_LOAD)
        {
            uint64_t begin = phdr.vaddr;
            uint64_t size = phdr.file_sz;

            for (uint32_t i = 0; i < size; i += PAGE_SIZE_4K)
            {
                void* phys = pmm_request();
                memcpy(phys, elf_dat + phdr.offset + i * PAGE_SIZE_4K, PAGE_SIZE_4K);
                page_map_memory(phdr.vaddr + i * PAGE_SIZE_4K, phys, 1, proc->addr_space);
            }

            //page_alloc_region(begin, size / PAGE_SIZE_4K + 1, proc->addr_space);

            

            //exec_base = (uint64_t)kmalloc(phdr.file_sz);

            //memcpy((void*)(exec_base + phdr.vaddr), elf_dat + phdr.offset, phdr.file_sz);
            //memcpy((void*)(exec_base), elf_dat + phdr.offset, phdr.file_sz);

        }
    }

    proc->regs.rip = hdr.entry;
}

proc_t* mk_elf_proc(uint8_t* elfdat)
{
    proc_t* proc = kmalloc(sizeof(proc_t));

    proc->next = NULL;
    proc->pid = 0;
    proc->sleep_exp = 0;
    proc->state = PROC_STATE_READY;
    proc->addr_space = page_mk_map(); // Creates a virtual address space with kernel mapped
    proc->file_descs = list_create();

    // TODO: These are temporary - later will be hooked up to PTYs
    vfs_node_t* stdin_node = vfs_resolve_path("/dev/stdout", NULL);
    fs_fd_t* stdin = vfs_open(stdin_node, 0, 0);
    list_push_back(proc->file_descs, stdin);

    vfs_node_t* stdout_node = vfs_resolve_path("/dev/stdout", NULL);
    fs_fd_t* stdout = vfs_open(stdout_node, 0, 0);
    list_push_back(proc->file_descs, stdout);

    page_alloc_region(0x20000, 0x4000, proc->addr_space);

    memset(&(proc->regs), 0, sizeof(reg_ctx_t));
    loadelf(elfdat, proc);
    proc->regs.rflags = 0x202;
    proc->regs.cs = KERNEL_CS;
    proc->regs.ss = KERNEL_SS;
    proc->regs.rbp = (uint64_t)0x24000;
    proc->regs.rsp = (uint64_t)0x24000;

    return proc;
}