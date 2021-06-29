#include <stdio.h>
#include <string.h>

int main(int argc, char** argv)
{
    system("dd if=/dev/zero of=boot.img bs=1k count=14400");
    //system("mkfs.fat -F 16 -I boot.img");
    system("mformat -i boot.img -f 1440 ::");
    system("mcopy -i boot.img ../../../../kernel/kernel.elf ::");
    system("mcopy -i boot.img loader.bin ::");

    FILE* dst = fopen("boot.img", "rb+");
    FILE* src = fopen("mbr.bin", "rb");

    char buffer[512];
    fread(buffer, 512, 1, dst);

    buffer[0] = 0xeb; // JMP
    buffer[1] = 0x3e; // 0x3E
    buffer[2] = 0x90; // NOP

    char code[448];
    fseek(src, 0x3e, SEEK_SET);
    fread(code, 448, 1, src);
    memcpy(buffer + 0x3e, code, 448);

    buffer[510] = 0x55;
    buffer[511] = 0xaa;

    fseek(dst, 0, SEEK_SET);
    fwrite(buffer, 512, 1, dst);

    fclose(dst);
    fclose(src);

    return 0;
}