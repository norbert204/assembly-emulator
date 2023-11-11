#include "ram_file.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include "../base/types.h"
#include "../base/string_utils.h"
#include "segment_data.h"
#include "segment_text.h"

#define LINE_SIZE 255

#define MAIN_FUNCTION_NAME " <main>:"

void set_state(char *line, segment *seg, FILE *out)
{
    if (strstr(line, ".text"))
    {
        *seg = TEXT;
        fprintf(out, ".text\n");
    }
    else if (strstr(line, ".data")
        || strstr(line, ".rodata")
        || strstr(line, ".bss")
        || strstr(line, "rodata"))
    {
        char segment[16];
        sscanf(line, "Disassembly of section %[^:]", segment); 

        *seg = DATA;
        fprintf(out, "%s\n", segment);
    }
    else 
    {
        *seg = NO_SEGMENT;
    }
}

int get_main_address(FILE *in)
{
    char line[LINE_SIZE];
    bool found_main = false;

    while (fgets(line, LINE_SIZE, in) != NULL)
    {
        if (strstr(line, MAIN_FUNCTION_NAME))
        {
            found_main = true;
            break;
        }
    }

    if (!found_main)
    {
        fprintf(stderr, "Main function not found in executable.\n");
        exit(1);
    }

    int address;
    sscanf(line, "%x", &address);

    fseek(in, 0, SEEK_SET);

    return address;
}

void create_ram_file(const char *input_file, const char *output_file)
{
    FILE *in = fopen(input_file, "r");
    FILE *out = fopen(output_file, "w");

    int main_address = get_main_address(in);
    fprintf(out, "main: %x\n", main_address);

    char line[LINE_SIZE];

    segment cur_segment = NO_SEGMENT;

    while (fgets(line, LINE_SIZE, in) != NULL)
    {
        if (str_starts_with(line, "Disassembly of"))
        {
            set_state(line, &cur_segment, out);
            continue;
        }

        if (cur_segment == TEXT)
        {
            handle_text_segment(in, out);
        }
        else if (cur_segment != NO_SEGMENT)
        {
            handle_data_segments(cur_segment, in, out);
        }
    }

    fclose(in);
    fclose(out);
}
