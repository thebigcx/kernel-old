# Switch current task

.global ctx_switch

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

ctx_switch:
    movq    %rdi, %rsp
    movq    %rsi, %rax
    popaq

    movq    %rax, %cr3

    popq    %rax
    iretq