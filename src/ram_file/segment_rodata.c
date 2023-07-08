#include "segment_rodata.h"
#include <stdbool.h>
#include "../base/string_utils.h"

void print_line_to_file2(FILE *out, int address, char *byte)
{
    fprintf(out, "%x\t%s\n", address, byte);
}

void handle_rodata_segment(FILE *in, FILE *out)
{
    char line[512];
    long last_file_pos = ftell(in);

    int address;
    char bytes[400];

    memset(line, '\0', 512);
    memset(bytes, '\0', 400);

    while (true)
    {
        fgets(line, 512, in);

        if (str_starts_with(line, "Disassembly of"))
        {
            fseek(in, last_file_pos, SEEK_SET);
            break;
        }

        if (str_char_count(line, 512, '\t') < 2)
        {
            continue;
        }

        sscanf(line, "%x:\t%[^\t\n]\t%*[^\n]", &address, bytes);
        str_trim(bytes, 400);

        char *byte;

        for (byte = strtok(bytes, " "); byte != NULL; byte = strtok(NULL, " "))
        {
            print_line_to_file2(out, address, byte);
            address++;
        }

        last_file_pos = ftell(in);
    }
}
