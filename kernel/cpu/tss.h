#pragma once

#include <util/types.h>

// Task State Segment
typedef struct tss_ent
{
    uint32_t res1;
    uint32_t rsp0l;
    uint32_t rsp0h;
    uint32_t rsp1l;
    uint32_t rsp1h;
    uint32_t rsp2l;
    uint32_t rsp2h;
    uint64_t res2;
    uint32_t ist1l;
    uint32_t ist1h;
    uint32_t ist2l;
    uint32_t ist2h;
    uint32_t ist3l;
    uint32_t ist3h;
    uint32_t ist4l;
    uint32_t ist4h;
    uint32_t ist5l;
    uint32_t ist5h;
    uint32_t ist6l;
    uint32_t ist6h;
    uint32_t ist7l;
    uint32_t ist7h;
    uint64_t res3;
    uint16_t res4;
    uint16_t iomap_base;

} __attribute__((packed)) tss_ent_t;

void tss_init(uint32_t idx, uint64_t krsp);