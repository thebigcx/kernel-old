.section .text

.set SMP_TRAMPOLINE_CR3,   0x1000
.set SMP_TRAMPOLINE_GDT,   0x1008
.set SMP_TRAMPOLINE_ENTRY, 0x1010
.set SMP_TRAMPOLINE_STACK, 0x1018

.global _ap_bootstrap_start
_ap_bootstrap_start:

.code16
ap_trampoline:
    cli
    cld
    ljmp    $0x800, $(1f - ap_trampoline)
1:
    movw    %cs, %ax
    movw    %ax, %ds 

    lgdtl   (gdtptr32 - ap_trampoline)

    movl    %cr0, %eax
    orl     $1, %eax
    movl    %eax, %cr0

    ljmpl   $0x8, $(smpentry32 - ap_trampoline + 0x8000)

.code32
smpentry32:
    movw    $0x10, %ax
    movw    %ax, %ds
    movw    %ax, %ss
    movw    %ax, %es
    movw    %ax, %fs
    movw    %ax, %gs

    lgdtl   (gdtptr64 - ap_trampoline + 0x8000)

    movl    $0xc0000080, %ecx
    rdmsr
    orl     $0x800, %eax
    wrmsr

    mov     SMP_TRAMPOLINE_CR3, %eax
    mov     %eax, %cr3

    movl    $0xc0000080, %ecx
    rdmsr
    orl     $(1 << 8), %eax
    wrmsr

    movl    %cr4, %eax
    orl     $0x60, %eax
    movl    %eax, %cr4

    movl    %cr0, %eax
    orl     $0x80000000, %eax
    movl    %eax, %cr0
    
    ljmpl   $0x8, $(smpentry64 - ap_trampoline + 0x8000)

.code64
smpentry64:
    movq    $SMP_TRAMPOLINE_STACK, %rsp
    xorq    %rbp, %rbp

    mov     $1, %eax
    cpuid
    shrl    $24, %ebx
    movl    %ebx, %edi
    movq    $SMP_TRAMPOLINE_ENTRY, %rax
    jmp     *(%rax)

    hlt // Should not reach, but just in case

.align 16
gdt32:
    // Null
    .8byte 0x0

    .4byte 0x0000ffff 
    .4byte 0x00cf9a00

    .4byte 0x0000ffff
    .4byte 0x00cf9200
    
gdt32_end:

gdtptr32:
    .2byte gdt32_end - gdt32 - 1
    .4byte (gdt32 - ap_trampoline) + 0x8000

.align 16
gdt64:
    // Null
    .8byte 0x0

    .4byte 0x00000000
    .4byte 0x00209800

    .4byte 0x00000000
    .4byte 0x00009200
gdt64_end:

gdtptr64:
    .2byte gdt64_end - gdt64 - 1
    .8byte (gdt64 - ap_trampoline) + 0x8000

.global _ap_bootstrap_end
_ap_bootstrap_end: