# Load Global Descriptor Table

.global gdt_flush

gdt_flush:
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

.global tss_flush

tss_flush:
	mov $0x2b, %ax
	ltr %ax
	ret
