#pragma once

typedef volatile int lock_t;
/*
#define acquire_lock(name)\
    while (!__sync_bool_compare_and_swap(&name, 0, 1)); __sync_synchronize();

#define release_lock(name)\
    __sync_synchronize(); name = 0;*/

#define acquire_lock(name)\
	while (__sync_lock_test_and_set(&name, 1)) asm ("pause");

#define acquire_test_lock(name) ({ int stat; stat = __sync_lock_test_and_set(&name, 1); stat; })

#define release_lock(name)\
	__sync_lock_release(&name);
