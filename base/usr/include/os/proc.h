#pragma once

int proc_fork();
int proc_exec(const char* path, int argc, char** argv);
int proc_waitpid(int pid);
void proc_exit(int code);