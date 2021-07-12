#pragma once

#include <stdint.h>

typedef uint64_t thread_t;

int thread_creat(thread_t* thread, void* entry, void* arg);
void thread_exit();

void thread_sleepns(uint64_t ns);
void thread_sleepus(uint64_t us);
void thread_sleepms(uint64_t ms);
void thread_sleeps(uint64_t s);