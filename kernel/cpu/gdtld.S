# Load Global Descriptor Table

.global gdt_load

gdt_load:
    lgdt    (%rdi)
    mov     $0x10, %ax
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs
    mov     %ax, %ss
    pop     %rdi
    mov     $0x08, %rax
    pushq   %rax
    pushq   %rdi
    retfq
