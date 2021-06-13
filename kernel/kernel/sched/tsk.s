# Switch current task

.global task_switch
.global save_regs

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

task_switch:
    movq    %rdi, %rsp
    movq    %rsi, %rax
    popaq

    movq    %rax, %cr3

    popq    %rax
    iretq
