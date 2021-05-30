#include <sched/sched.h>
#include <stddef.h>
#include <paging/paging.h>
#include <gdt/idt.h>
#include <drivers/input/keyboard/ps2_keyboard.h>
#include <drivers/input/mouse/ps2_mouse.h>
#include <elf.h>
#include <drivers/fs/vfs/vfs.h>
#include <stdint.h>
#include <string.h>

proc_t* ready_lst_start;
proc_t* ready_lst_end;
proc_t* last_proc;

extern void kernel_proc();

void test_proc_1()
{
    puts("PROC1");
    for (;;)
    {
        uint32_t key;
        while (kb_get_key(&key))
        {
            puts("Keyboard\n");
        }
    }
}
void test_proc_2()
{
    puts("PROC2");
    for (;;)
    {
        mouse_packet_t pack;
        while (mouse_get_packet(&pack))
        {
            puts("Mouse\n");
        }
    }
}

void sched_init()
{
    asm ("sti");
    for (;;);
}

void schedule(reg_ctx_t* r)
{
    if (ready_lst_start != NULL)
    {
        last_proc->regs = *r;
        proc_t* task = ready_lst_start;
        ready_lst_start = task->next;
        last_proc = task;
        task_switch(&(task->regs), task->addr_space);
    }
}

proc_t* mk_proc(void* entry)
{
    proc_t* proc = page_request();
    //proc_t* proc = kmalloc(sizeof(proc_t));
    proc->state = PROC_STATE_READY;
    proc->addr_space = page_get_kpml4(); // TODO: use page_mk_map()
    proc->pid = 0;
    proc->next = NULL;

    /*void* stack = page_request();
    page_kernel_map_memory(stack, stack);
    for (int i = 1; i < 4; i++)
    {
        page_kernel_map_memory(stack + i * PAGE_SIZE, page_request());
    }*/
    void* stack = kmalloc(16000);
    memset(stack, 0, 16000);

    memset(&(proc->regs), 0, sizeof(reg_ctx_t));
    proc->regs.rip = entry;
    proc->regs.rflags = 0x202;
    proc->regs.cs = KERNEL_CS;
    proc->regs.ss = KERNEL_SS;
    proc->regs.rbp = (uint64_t)stack + 16000;
    proc->regs.rsp = (uint64_t)stack + 16000;

    return proc;
}

proc_t* mk_elf_proc(uint8_t* elf_dat)
{
    elf64_hdr_t hdr;
    elf64_program_hdr_t phdr;
    uint64_t base = UINT64_MAX;

    memcpy(&hdr, elf_dat, sizeof(elf64_hdr_t));

    if (hdr.ident[0] != 0x7f || hdr.ident[1] != 'E' || hdr.ident[2] != 'L' || hdr.ident[3] != 'F')
    {
        return NULL;
    }

    /*uint64_t size = 0;

    for (int i = 0; i < hdr.ph_num; i++)
    {
        memcpy(&phdr, elf_dat + hdr.ph_off + hdr.ph_ent_size * i, sizeof(elf64_program_hdr_t));
        base = base < phdr.vaddr ? base : phdr.vaddr;
        uint64_t seg_end = phdr.vaddr - base + phdr.mem_sz;
        size = size > seg_end ? size : seg_end;
    }

    //uint8_t* final_img = kmalloc(size);
    uint8_t* final_img = page_request();

    for (int i = 0; i < hdr.ph_num; i++)
    {
        uint64_t addr;
        memcpy(&phdr, elf_dat + hdr.ph_off + hdr.ph_ent_size * i, sizeof(elf64_program_hdr_t));
        addr = (phdr.vaddr - base) + (uint64_t)final_img;
        memset(addr, 0, phdr.mem_sz);
        memcpy(addr, elf_dat + phdr.offset, phdr.file_sz);
    }

    return mk_proc(hdr.entry + final_img);*/
}

void sched_tick(reg_ctx_t* r)
{
    outb(PIC1_COMMAND, PIC_EOI);
    schedule(r);
}

void sched_spawn_proc(proc_t* proc)
{
    if (!ready_lst_start)
    {
        ready_lst_start = proc;
        return;
    }

    if (!ready_lst_end)
    {
        ready_lst_start = proc;
        ready_lst_end = proc;
        return;
    }

    proc_t* last = ready_lst_end;
    last->next = proc;
    ready_lst_end = proc;
}