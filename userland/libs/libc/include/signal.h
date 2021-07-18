#pragma once

#include <sys/types.h>

#define SIGINT  0
#define SIGSEG  1
#define SIGABRT 2

int kill(pid_t pid, int sig);
int raise(int sig);