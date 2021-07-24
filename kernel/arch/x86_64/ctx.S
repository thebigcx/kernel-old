# Switch current task

.global arch_enter_kernel
.global arch_enter_user

.macro popaq

    pop     %r15
    pop     %r14
    pop     %r13
    pop     %r12
    pop     %r11
    pop     %r10
    pop     %r9
    pop     %r8
    pop     %rbp
    pop     %rdi
    pop     %rsi
    pop     %rdx
    pop     %rcx
    pop     %rbx

.endm

arch_enter_kernel:
    movq    %rdi, %rsp
    movq    %rsi, %rax
    popaq

    movq    %rax, %cr3

    popq    %rax
    // Stack contains RSP, CS, RIP, RFLAGS, SS
    iretq

arch_enter_user:
    mov     $0x23, %ax
    mov     %ax, %ds
    mov     %ax, %es
    mov     %ax, %fs
    mov     %ax, %gs

    movq    %rdi, %rsp
    movq    %rsi, %rax
    popaq

    movq    %rax, %cr3

    popq    %rax
    // Stack contains RSP, CS, RIP, RFLAGS, SS
    iretq
