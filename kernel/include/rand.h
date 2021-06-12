#pragma once

#include <stdint.h>

#define RAND_MAX INT32_MAX

void rand_seed(uint32_t seed);
uint64_t rand_range(uint64_t min, uint64_t max);