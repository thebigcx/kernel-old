.global jump_usermode

jump_usermode:
    mov $0x23, %ax
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    mov %rsp, %rax
    push $0x23
    push %rax
    pushf
    push $0x1b
    lea user_start, %rax
    push %rax
    iret

user_start:
    add $4, %esp
    ret