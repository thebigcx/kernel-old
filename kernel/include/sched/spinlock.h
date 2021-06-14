#pragma once

typedef volatile int lock_t;

#define acquire_lock(name)\
    ({ while (__sync_lock_test_and_set(name, 1)) asm ("pause"); })

#define release_lock(name)\
    ({ __sync_lock_release(name); })