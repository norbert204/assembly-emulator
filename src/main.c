#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ram_file/compile.h"
#include "ram_file/decompile.h"
#include "emulator/x86_64.h"
#include "ram_file/ram_file.h"

#define OUTPUT_DUMP_FILE "/tmp/asemu_dump"
#define OUTPUT_RAM_FILE "/tmp/asemu_ram"
#define OUTPUT_EXECUTABLE "/tmp/asemu_executable"

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
    puts("\tasemu [parameters] <file>");
    puts("");
    puts("Optional parameters");
    puts("\t-h\tPrint this help text");
    puts("\t-c\tPrint credits");
    puts("\t-e\tThe provided file is a precompiled executable");
    puts("\t-r\tThe provided file is a premade ram file");
}

void print_file_specifier_error()
{
    fprintf(stderr, "Provide only one file type specifying parameter!\n");
    exit(1);
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

    int run_state = 0;

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

        if (strstr(argv[i], "-e"))
        {
            if (run_state != 0)
            {
                print_file_specifier_error();
            }

            run_state = 1;
            continue;
        }

        if (strstr(argv[i], "-r"))
        {
            if (run_state != 0)
            {
                print_file_specifier_error();
            }

            run_state = 2;
            continue;
        }
    }

    char *file = argv[argc - 1];

    if (run_state == 0)
    {
        compile_code(file, OUTPUT_EXECUTABLE);
        decompile_executable(OUTPUT_EXECUTABLE, OUTPUT_DUMP_FILE);
        create_ram_file(OUTPUT_DUMP_FILE, OUTPUT_RAM_FILE);
        emulation();
    }
    else if (run_state == 1)
    {
        decompile_executable(file, OUTPUT_DUMP_FILE);
        create_ram_file(OUTPUT_DUMP_FILE, OUTPUT_RAM_FILE);
        emulation();
    }
    else if (run_state == 2)
    {
        // TODO: Make the emulator have an input file parameter.
        char command[256];
        sprintf(command, "cp %s /tmp/asemu_ram", file);
        system(command);

        emulation();
    }

    return 0;
}
