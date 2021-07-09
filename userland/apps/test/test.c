#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <os/keyboard.h>
#include <sys/stat.h>

static int keyboard;

void runcmd(char* cmd)
{
    printf("\n");

    char bin[50];
    strncpy(bin, "/bin/", 5);

    // Extract the command name
    int i = 0;
    for (; cmd[i] != ' ' && cmd[i] != 0; i++)
    {
        bin[i + 5] = cmd[i];
    }

    bin[i + 5] = 0;

    stat_t binstat;
    if (stat(bin, &binstat) != 0)
    {
        printf("%s: command not found\n", bin);
        return;
    }

    /*int pid = fork();
    if (pid == 0)
    {
        exec(bin, 0, NULL);
    }
    else
    {
        waitpid(pid);
    }*/
    //exec(bin, 0, NULL);
    //fork();
}

static char scancode_toascii[] =
{
    'c', '~', '1', '2', '3', '4', '5',
    '6', '7', '8', '9', '0', '-', '=',
    '~', '~', 'q', 'w', 'e', 'r', 't',
    'y', 'u', 'i', 'o', 'p', '[', ']',
    '~', '~', 'a', 's', 'd', 'f', 'g',
    'h', 'j', 'k', 'l', ';', '\'', '`',
    '~', '\\', 'z', 'x', 'c', 'v', 'b',
    'n', 'm', ',', '.', '/', '~', '*',
    '~', ' ', '~', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c',
    'c', 'c', 'c', 'c', 'c', 'c', 'c'
};

void getcmd()
{
    asm volatile ("hlt");
    int fb = open("/dev/fb", 0, 0);
    void* addr = mmap(NULL, 0, 0, 0, 0, 0);
    *((uint32_t*)addr) = 0xffffffff;

    char cmd[200];
    int cmdlen = 0;

    printf(">> ");

    while (1)
    {
        uint32_t scancode;
        if (read(keyboard, &scancode, 1))
        {
            if (scancode == KEY_ENTER)
            {
                if (cmdlen == 0)
                {
                    printf("\n");
                    return;
                }

                cmd[cmdlen] = 0; // Null terminate
                runcmd(cmd);
                return;
            }
            else if (scancode < 88) // Key press
            {
                char c = scancode_toascii[scancode];
                cmd[cmdlen++] = c;
                write(1, &c, 1);
            }
        }
    }
}

void _start(int argc, char** argv)
{
    keyboard = open("/dev/keyboard", 0, 0);

    printf("Minimal bash-like shell - type 'help' for a list of commands\n\n");
    
    while (1)
    {
        getcmd();
    }

    exit(0);
    for (;;);
}