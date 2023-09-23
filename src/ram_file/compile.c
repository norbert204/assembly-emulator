#include "compile.h"

#include <stdio.h>
#include <stdlib.h>
#include "../base/string_utils.h"

void compile_code(const char *input_file, const char *output_file)
{
    char command[256];

    // We need to use different GCC parameters based on the file type.
    if (str_ends_with(input_file, ".s"))
    {
        sprintf(command, "gcc -no-pie %s -o %s", input_file, output_file);
    }
    else if (str_ends_with(input_file, ".c"))
    {
        sprintf(command, "gcc %s -o %s", input_file, output_file);
    }
    else
    {
        fprintf(stderr, "Cannot compile file '%s'. Unsupported file type detected!\n", input_file);
        exit(1);
    }

    system(command);
}
