.code16

.set SMP_TRAMPOLINE_CR3, 0x1000
.set SMP_TRAMPOLINE_GDT, 0x1008
.set SMP_TRAMPOLINE_ENTRY, 0x1010

.global _ap_bootstrap_start
_ap_bootstrap_start:

ap_trampoline:
    cli
    cld
    movl    $SMP_TRAMPOLINE_CR3, %eax
    movl    %eax, %cr3

    movl    $SMP_TRAMPOLINE_GDT, %eax
    lgdt    (%eax)

    ljmp    $8, $8000

.code64
smpentry64:
    movw    $0x10, %ax
    movw    %ax, %ds
    movw    %ax, %ss
    mov     $1, %eax
    cpuid
    shrl    $24, %ebx
    movl    %ebx, %edi
    movl    $SMP_TRAMPOLINE_ENTRY, %eax
    call    (%eax)

.global _ap_bootstrap_end
_ap_bootstrap_end: