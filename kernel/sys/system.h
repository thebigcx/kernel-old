#pragma once

#include <util/types.h>

typedef struct reg_ctx
{
    uint64_t r15;
    uint64_t r14;
    uint64_t r13;
    uint64_t r12;
    uint64_t r11;
    uint64_t r10;
    uint64_t r9;
    uint64_t r8;
    uint64_t rbp;
    uint64_t rdi;
    uint64_t rsi;
    uint64_t rdx;
    uint64_t rcx;
    uint64_t rbx;
    uint64_t rax;
    uint64_t rip;
    uint64_t cs;
    uint64_t rflags;
    uint64_t rsp;
    uint64_t ss;

} reg_ctx_t;

typedef struct isr_frame
{
    reg_ctx_t regs;
    uint32_t errcode;

} isr_frame_t;

void panic(const char* msg, uint64_t num, isr_frame_t* r);

// Shorthands for assembly

#define cli() asm ("cli")
#define sti() asm ("sti")
#define hlt() asm ("hlt")

void breakpoint();