#include <sched/sched.h>
#include <stddef.h>
#include <paging/paging.h>
#include <gdt/idt.h>
#include <drivers/input/keyboard/ps2_keyboard.h>

proc_t* ready_lst_start;
proc_t* ready_lst_end;
proc_t* last_proc;
int irq_disable_cnt = 0;
int tsk_post_flag = 0;

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
    for (;;);
}

void sched_init()
{
    proc_t* kproc = mk_proc(test_proc_1);
    ready_lst_start = kproc;
    proc_t* kproc2 = mk_proc(kernel_proc);
    kproc->next = kproc2;
    kproc2->next = kproc;
    //
    //kproc->next = kproc2;
    ready_lst_end = kproc;
    for (;;);
}

void schedule(reg_ctx_t* r)
{
    if (ready_lst_start != NULL)
    {
        //save_regs(&(last_proc->regs));
        last_proc->regs = *r;
        proc_t* task = ready_lst_start;
        ready_lst_start = task->next;
        last_proc = task;
        task_switch(&(task->regs), task->addr_space);
    }
}

void sched_lock()
{
    asm volatile ("cli");
    irq_disable_cnt++;
}

void sched_unlock()
{
    irq_disable_cnt--;
    if (irq_disable_cnt == 0)
        asm volatile ("sti");
}

proc_t* mk_proc(void* entry)
{
    proc_t* proc = page_request();
    proc->state = PROC_STATE_READY;
    proc->addr_space = page_get_kpml4(); // TODO: use page_mk_map()
    proc->pid = 0;
    proc->next = NULL;

    void* stack = page_request();
    page_kernel_map_memory(stack, stack);
    for (int i = 1; i < 4; i++)
    {
        page_kernel_map_memory(stack + i * PAGE_SIZE, page_request());
    }

    memset(&(proc->regs), 0, sizeof(reg_ctx_t));
    proc->regs.rip = entry;
    proc->regs.rflags = 0x202;
    proc->regs.cs = KERNEL_CS;
    proc->regs.ss = KERNEL_SS;
    proc->regs.rbp = (uint64_t)stack + PAGE_SIZE * 4;
    proc->regs.rsp = (uint64_t)stack + PAGE_SIZE * 4;

    return proc;
}

void sched_tick(reg_ctx_t* r)
{
    outb(PIC1_COMMAND, PIC_EOI);
    schedule(r);
}