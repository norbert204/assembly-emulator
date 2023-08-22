#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ram_file/ram_file.h"

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

int main(int argc, char *argv[])
{
    if (argc == 1)
    {
        print_help();
        exit(1);
    }

    // Handle the optional parameters
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

    printf("File to process: %s", argv[argc - 1]);

    create_ram_file(argv[argc - 1]);

    return 0;
}
