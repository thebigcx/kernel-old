.extern isr_handler
.extern irq_handler

.macro pushaq
    push    %rax
    push    %rbx
    push    %rcx
    push    %rdx
    push    %rsi
    push    %rdi
    push    %rbp
    push    %r8
    push    %r9
    push    %r10
    push    %r11
    push    %r12
    push    %r13
    push    %r14
    push    %r15
.endm

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
    pop     %rax
.endm

.macro ISR num

.global isr\num

isr\num:
    cli
    pushaq
    movq    $\num, %rdi
    movq    %rsp, %rsi
    xor     %rdx, %rdx
    xor     %rbp, %rbp
    call    isr_handler
    popaq
    iretq

.endm

.macro IRQ num, intnum

.global irq\num

irq\num:
    cli
    pushaq
    movq    $\intnum, %rdi
    movq    %rsp, %rsi
    xor     %rdx, %rdx
    xor     %rbp, %rbp
    call    irq_handler
    popaq
    iretq

.endm

.extern syscall_handler
.global isr0x80
isr0x80:
    cli
    pushaq
    movq    %rsp, %rdi
    xor     %rbp, %rbp
    call    syscall_handler
    popaq
    iretq

ISR 0
ISR 1
ISR 2
ISR 3
ISR 4
ISR 5
ISR 6
ISR 7
ISR 8
ISR 9
ISR 10
ISR 11
ISR 12
ISR 13
ISR 14
ISR 15
ISR 16
ISR 17
ISR 18
ISR 19
ISR 20
ISR 21
ISR 22
ISR 23
ISR 24
ISR 25
ISR 26
ISR 27
ISR 28
ISR 29
ISR 30
ISR 31
ISR 32

IRQ 0, 32
IRQ 1, 33
IRQ 2, 34
IRQ 3, 35
IRQ 4, 36
IRQ 5, 37
IRQ 6, 38
IRQ 7, 39
IRQ 8, 40
IRQ 9, 41
IRQ 10, 42
IRQ 11, 43
IRQ 12, 44
IRQ 13, 45
IRQ 14, 46
IRQ 15, 47
