#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ram_file/decompile.h"
#include "emulator/x86_64.h"
#include "ram_file/ram_file.h"

#define OUTPUT_DUMP_FILE "/tmp/asemu_dump"
#define OUTPUT_RAM_FILE "/tmp/asemu_ram"

void print_credits()
{
    puts("Assembly Emulator");
    puts("Made by:");
    puts("\tHorváth Norbert");
    puts("\tRóth Arnold");
    puts("\tSajó Balázs");
}

void print_help()
{
    puts("Usage:");
    puts("\tasemu [parameters] <executable>");
    puts("");
    puts("Optional parameters");
    puts("\t-h\tPrint this help text");
    puts("\t-c\tPrint credits");
}

void emulation()
{
    Init();

    int count = 1;

    while(Run)
    {
          SaveState();
          InstructionFetch();
          printf("%d --- %s\n", count, Mnemonic);
          OperandFetch();
          Execute();
          WriteBack();
          count++;
    }

    Fini();
}

int main(int argc, char *argv[])
{

    if (argc == 1)
    {
        print_help();
        exit(1);
    }

    // Handle the optional parameters.
    for (int i = 0; i < argc; i++)
    {
        if (strstr(argv[i], "-c"))
        {
            print_credits();
            exit(0);
        }

        if (strstr(argv[i], "-h"))
        {
            print_help();
            exit(0);
        }
    }

    char *file = argv[argc - 1];

    // First we need to decompile the executable.
    decompile_executable(file, OUTPUT_DUMP_FILE);

    // Then we create the RAM file.
    create_ram_file(OUTPUT_DUMP_FILE, OUTPUT_RAM_FILE);

    // Then run the emulator.
    emulation();

    return 0;
}
