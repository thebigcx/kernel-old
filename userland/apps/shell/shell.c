#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include <os/input.h>
#include <os/fs.h>

void runcmd(char* cmd)
{
    printf("\n");

    if (strcmp(cmd, "help") == 0)
    {
        printf(
            "Commands:\n"
            "\tls\n"
            "\tcat\n"
        );
        return;
    }

    char bin[50];
    strncpy(bin, "/usr/bin/", 9);
    char argv[50];
    int argc;

    // Extract the command name
    int i = 0;
    while (*cmd != ' ' && *cmd != 0 && *cmd != '\t')
    {
        bin[i + 9] = *cmd++;
        i++;
    }
    
    bin[i + 9] = 0;

    if (*cmd != 0)
    {
        *cmd++;

        i = 0;
        // First argument
        while (*cmd != ' ' && *cmd != 0)
        {
            argv[i] = *cmd++;
            i++;
        }
        argv[i] = 0;
    }

    stat_t binstat;
    if (file_stat(bin, &binstat) != 0)
    {
        printf("%s: command not found\n", bin);
        return;
    }

    char* argv_ptr = &argv;
    proc_exec(bin, 1, &argv_ptr);
}

void getcmd()
{
    char cmd[200];
    int cmdlen = 0;

    printf(">> ");

    while (1)
    {
        char c;
        if (read(0, &c, 1)) // read from stdin
        {
            if (c == '\n')
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
            else if (c == '\b')
            {
                putchar(c);
                cmdlen--;
            }
            else // Key press
            {
                cmd[cmdlen++] = c;
                putchar(c);
            }
        }
    }
}

int main(int argc, char** argv)
{
    printf("Minimal bash-like shell - type 'help' for a list of commands\n\n");
    
    while (1)
    {
        getcmd();
    }

    return 0;
}