#pragma once

#include <cpu/cpu.h>

extern cpu_t cpus[64];

void smp_init();
void smp_initcpu(uint32_t id);