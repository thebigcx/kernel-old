#include <unistd.h>
#include <os/proc.h>

void exit(int status)
{
    proc_exit(status);
}

int execve(const char* pathname, char* const argv[], char* const envp[])
{
    
}