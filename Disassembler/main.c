#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include "DisOpcode.c"

int main(int argc, char **argv)
{
    FILE *rom = fopen(argv[1], "rb");
    if (rom == NULL)
    {
        printf("File cannot be found");
    }

    fseek(rom, 0L, SEEK_END);

    int pc = 0;
    int fsize = ftell(rom);

    uint8_t *buffer = malloc(fsize);

    fseek(rom, 0L, SEEK_SET);

    fread((void *)buffer, fsize, 1, rom);
    fclose(rom);

    while (pc < 26) // fsize)
    {
        pc += DisassembleOpcode(buffer, pc);
    }
    return 0;
}