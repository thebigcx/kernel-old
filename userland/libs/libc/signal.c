#include <signal.h>
#include <os/proc.h>
#include <unistd.h>

int kill(pid_t pid, int sig)
{
    sigsend((int)pid, sig);
    return 0;
}

int raise(int sig)
{
    kill((int)getpid(), sig);
    return 0;
}