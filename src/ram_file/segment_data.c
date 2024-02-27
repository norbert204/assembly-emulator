#include "segment_data.h"
#include <stdlib.h>
#include <regex.h>
#include "../base/string_utils.h"

void print_line_to_file(FILE *out, const int address, const char *bytes)
{
    char bytes_copy[400];
    memset(bytes_copy, '\0', 400);
    strncpy(bytes_copy, bytes, 400);

    int address_copy = address;

    char *byte;

    for (byte = strtok(bytes_copy, " "); byte != NULL; byte = strtok(NULL, " "))
    {
        fprintf(out, "%x\t%s\n", address_copy, byte);
        address_copy++;
    }
}

void handle_data_segments(segment segment, FILE *in, FILE *out)
{
    char line[512];
    long last_file_pos = ftell(in);

    char bytes[400];

    memset(line, '\0', 512);
    memset(bytes, '\0', 400);

    while (!feof(in))
    {
        fgets(line, 512, in);

        // We don't need to handle empty lines.
        if (line[0] == '\n')
        {
            last_file_pos = ftell(in);
            continue;
        }

        if (str_starts_with(line, "Disassembly of"))
        {
            fseek(in, last_file_pos, SEEK_SET);
            break;
        }

        regex_t regex;
        regcomp(&regex, "^([:number:]*) (.*):", REG_EXTENDED);
        int result = regexec(&regex, line, 0, NULL, 0);

        if (result)
        {
            last_file_pos = ftell(in);
            continue;
        }

        int address;

        sscanf(line, "%x:\t%[^\t\n]\t%*[^\n]", &address, bytes);

        str_trim(bytes, 400);
        print_line_to_file(out, address, bytes);

        last_file_pos = ftell(in);
    }

    fseek(in, last_file_pos, SEEK_SET);
}
