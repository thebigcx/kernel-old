unsigned long syscall(unsigned long sysno, ...);

void _start()
{
    //asm volatile ("hlt");

    char buffer[100];
    buffer[0] = 'h';
    buffer[1] = 'e';
    buffer[2] = 'l';
    buffer[3] = 'l';
    buffer[4] = 'o';

    //syscall(1, 0, buffer, 100);
    for (;;);
}