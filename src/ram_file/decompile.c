#include "decompile.h"
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

// More information:
// https://stackoverflow.com/questions/66876343/check-if-file-is-a-binary-executable
#define REQUIRED_MAGIC_CONSTANT 0x464c457f

bool is_binary_executable(const char *path)
{
    if (access(path, F_OK) != 0)
    {
        fprintf(stderr, "Cannot access specified file! Please ensure that the file exists and has read permission for the current user.");
        exit(1);
    }

    int in = open(path, O_RDONLY);
    int magic_constant;

    int byte_count = read(in, &magic_constant, 4);
    close(in);

    if (byte_count != 4)
    {
        return false;
    }

    if (magic_constant != REQUIRED_MAGIC_CONSTANT)
    {
        return false;
    }

    return true;
}

void decompile_executable(const char *path, const char *output)
{
    bool good = is_binary_executable(path);

    if (!good)
    {
        fprintf(stderr, "File is not an executable binary!\n");
        exit(1);
    }

    char command[1024];
    sprintf(command, "objdump %s -D -M intel -z > %s", path, output);

    system(command);
}
