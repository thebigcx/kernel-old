#pragma once

#include <cpu/cpu.h>

extern cpu_t cpus[64];
extern uint32_t cpu_count;

void smp_init();
void smp_initcpu(uint32_t id);