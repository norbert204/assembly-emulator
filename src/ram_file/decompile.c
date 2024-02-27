#include "decompile.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <elf.h>

void is_binary_executable(const char *path)
{
    if (access(path, F_OK) != 0)
    {
        fprintf(stderr, "Cannot access specified file! Please ensure that the file exists and has read permission for the current user.\n");
        exit(1);
    }

    Elf64_Ehdr elf_header;

    FILE *file = fopen(path, "rb");

    if (!file)
    {
        fprintf(stderr, "Cannot read file!\n");
    }

    fread(&elf_header, sizeof(elf_header), 1, file);

    if (memcmp(elf_header.e_ident, ELFMAG, SELFMAG) != 0)
    {
        fclose(file);
        fprintf(stderr, "File is not an ELF binary!\n");
        exit(1);
    }

    // if (elf_header.e_type != ET_EXEC)
    // {
    //     fprintf(stderr, "File is not an executable!\n");
    //     return false;
    // }

    if (elf_header.e_machine != EM_X86_64)
    {
        fclose(file);
        fprintf(stderr, "File is not compiled for x86-64 architecture!\n");
        exit(1);
    }
}

void decompile_executable(const char *path, const char *output)
{
    is_binary_executable(path);

    char command[1024];
    sprintf(command, "objdump %s -D -M intel -z > %s", path, output);

    system(command);
}
