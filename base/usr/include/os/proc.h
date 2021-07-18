#pragma once

#define SIGINT  0
#define SIGSEG  1
#define SIGABRT 2

int proc_fork();
int proc_exec(const char* path, int argc, char** argv);
int proc_waitpid(int pid);
void proc_exit(int code);
void sigsend(int pid, int signal);
int proc_getpid();