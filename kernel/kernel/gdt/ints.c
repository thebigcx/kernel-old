#include <gdt/idt.h>
#include <io.h>
#include <stdio.h>
#include <system.h>
#include <input/keyboard.h>

__attribute__((interrupt)) void isr0(struct interrupt_frame* frame) { if (isrs[0]) isrs[0](); }
__attribute__((interrupt)) void isr1(struct interrupt_frame* frame) { if (isrs[1]) isrs[1](); }
__attribute__((interrupt)) void isr2(struct interrupt_frame* frame) { if (isrs[2]) isrs[2](); }
__attribute__((interrupt)) void isr3(struct interrupt_frame* frame) { if (isrs[3]) isrs[3](); }
__attribute__((interrupt)) void isr4(struct interrupt_frame* frame) { if (isrs[4]) isrs[4](); }
__attribute__((interrupt)) void isr5(struct interrupt_frame* frame) { if (isrs[5]) isrs[5](); }
__attribute__((interrupt)) void isr6(struct interrupt_frame* frame) { if (isrs[6]) isrs[6](); }
__attribute__((interrupt)) void isr7(struct interrupt_frame* frame) { if (isrs[7]) isrs[7](); }
__attribute__((interrupt)) void isr8(struct interrupt_frame* frame) { if (isrs[8]) isrs[8](); }
__attribute__((interrupt)) void isr9(struct interrupt_frame* frame) { if (isrs[9]) isrs[9](); }
__attribute__((interrupt)) void isr10(struct interrupt_frame* frame) { if (isrs[10]) isrs[10](); }
__attribute__((interrupt)) void isr11(struct interrupt_frame* frame) { if (isrs[11]) isrs[11](); }
__attribute__((interrupt)) void isr12(struct interrupt_frame* frame) { if (isrs[12]) isrs[12](); }
__attribute__((interrupt)) void isr13(struct interrupt_frame* frame) { if (isrs[13]) isrs[13](); outb(PIC1_COMMAND, PIC_EOI); }
__attribute__((interrupt)) void isr14(struct interrupt_frame* frame) { if (isrs[14]) isrs[14](); outb(PIC1_COMMAND, PIC_EOI); }

__attribute__((interrupt)) void irq0(struct interrupt_frame* frame) { if (irqs[0]) irqs[0](); outb(PIC1_COMMAND, PIC_EOI); }
__attribute__((interrupt)) void irq1(struct interrupt_frame* frame) { if (irqs[1]) irqs[1](); outb(PIC1_COMMAND, PIC_EOI); }
__attribute__((interrupt)) void irq2(struct interrupt_frame* frame) { if (irqs[2]) irqs[2](); }
__attribute__((interrupt)) void irq3(struct interrupt_frame* frame) { if (irqs[3]) irqs[3](); }
__attribute__((interrupt)) void irq4(struct interrupt_frame* frame) { if (irqs[4]) irqs[4](); }
__attribute__((interrupt)) void irq5(struct interrupt_frame* frame) { if (irqs[5]) irqs[5](); }
__attribute__((interrupt)) void irq6(struct interrupt_frame* frame) { if (irqs[6]) irqs[6](); }
__attribute__((interrupt)) void irq7(struct interrupt_frame* frame) { if (irqs[7]) irqs[7](); }
__attribute__((interrupt)) void irq8(struct interrupt_frame* frame) { if (irqs[8]) irqs[8](); }
__attribute__((interrupt)) void irq9(struct interrupt_frame* frame) { if (irqs[9]) irqs[9](); }
__attribute__((interrupt)) void irq10(struct interrupt_frame* frame) { if (irqs[10]) irqs[10](); }
__attribute__((interrupt)) void irq11(struct interrupt_frame* frame) { if (irqs[11]) irqs[11](); }
__attribute__((interrupt)) void irq12(struct interrupt_frame* frame) { if (irqs[12]) irqs[12](); outb(PIC2_COMMAND, PIC_EOI); outb(PIC1_COMMAND, PIC_EOI); }
__attribute__((interrupt)) void irq13(struct interrupt_frame* frame) { if (irqs[13]) irqs[13](); }
__attribute__((interrupt)) void irq14(struct interrupt_frame* frame) { if (irqs[14]) irqs[14](); }
__attribute__((interrupt)) void irq15(struct interrupt_frame* frame) { if (irqs[15]) irqs[15](); }