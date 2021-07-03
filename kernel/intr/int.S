.extern isr_handler
.extern irq_handler
.extern ipi_handler
.extern idtptr

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

.macro ISR_NOERR num

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

.macro ISR_ERR num

.global isr\num

isr\num:
    cli
    pushq   40(%rsp) # SS
    pushq   40(%rsp) # RSP
    pushq   40(%rsp) # RFLAGS
    pushq   40(%rsp) # CS
    pushq   40(%rsp) # RIP
    pushaq
    movq    $\num, %rdi
    movq    %rsp, %rsi
    movq    160(%rsp), %rdx
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

.macro IPI num

.global ipi\num

ipi\num:
    cli
    pushaq
    movq    $\num, %rdi
    movq    %rsp, %rsi
    xor     %rdx, %rdx
    xor     %rbp, %rbp
    call    ipi_handler
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

ISR_NOERR 0
ISR_NOERR 1
ISR_NOERR 2
ISR_NOERR 3
ISR_NOERR 4
ISR_NOERR 5
ISR_NOERR 6
ISR_NOERR 7
ISR_ERR 8
ISR_NOERR 9
ISR_ERR 10
ISR_ERR 11
ISR_ERR 12
ISR_ERR 13
ISR_ERR 14
ISR_NOERR 15
ISR_NOERR 16
ISR_ERR 17
ISR_NOERR 18
ISR_NOERR 19
ISR_NOERR 20
ISR_NOERR 21
ISR_NOERR 22
ISR_NOERR 23
ISR_NOERR 24
ISR_NOERR 25
ISR_NOERR 26
ISR_NOERR 27
ISR_NOERR 28
ISR_NOERR 29
ISR_ERR 30
ISR_NOERR 31
ISR_NOERR 32

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

IPI 0xfd
