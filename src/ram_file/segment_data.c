#include "segment_data.h"
#include <stdbool.h>
#include <stdlib.h>
#include <regex.h>
#include "../base/string_utils.h"

static int print_line_to_file(FILE *out, const int address, const char *bytes)
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

    return address_copy - 1;
}

void print_section(FILE *out, const int begin, const int end, const char *bytes, const size_t bytes_size)
{
    for (int i = begin + 1; i < end; i++)
    {
        i = print_line_to_file(out, i, bytes);
    }
}

void handle_data_segments(segment segment, FILE *in, FILE *out)
{
    char line[512];
    long last_file_pos = ftell(in);

    bool triple_dot = false;

    int address;
    int last_address;

    char bytes[400];
    char last_bytes[400];

    memset(line, '\0', 512);
    memset(bytes, '\0', 400);
    memset(last_bytes, '\0', 400);

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
            if (!triple_dot)
            {
                break;
            }

            int found_address = 0;

            while (!feof(in))
            {
                fgets(line, 512, in);

                if (line[0] != '\n')
                {
                    found_address = 1;
                    break;
                }
            }

            // If we didn't find an address, because we ran to the end of the file
            // we need to print 100 00 bytes to the file.
            if (!found_address)
            {
                memset(bytes, '\0', 400);
                strncpy(bytes, "00", 400);

                print_section(out, last_address, last_address + 100, bytes, 400);
                break;
            }

            sscanf(line, "%x%*[:^\n]", &address);

            // If we find an address that is lower that the last one, we should do
            // the same thing as before.
            if (address < last_address)
            {
                memset(bytes, '\0', 400);
                strncpy(bytes, "00", 400);

                print_section(out, last_address, last_address + 100, bytes, 400);
                break;
            }

            print_section(out, last_address, address, last_bytes, 400);
            break;
        }

        if (strstr(line, "..."))
        {
            triple_dot = true;
            last_file_pos = ftell(in);
            continue;
        }
        
        // We should only handle lines that doesn't have byte data
        // when we had a triple dot line before 
        regex_t regex;
        regcomp(&regex, "^([:number:]*) (.*):", REG_EXTENDED);
        int result = regexec(&regex, line, 0, NULL, 0);

        if (result && triple_dot)
        {
            sscanf(line, "%x%*[ ]", &address);

            memset(bytes, '\0', 400);
            strncpy(bytes, "00", 400);

            print_section(out, last_address, address, bytes, 400);
            triple_dot = false;

            last_address = address;
            strncpy(last_bytes, bytes, 400);
            last_file_pos = ftell(in);

            continue;
        }

        // Normal line with byte data
        sscanf(line, "%x:\t%[^\t\n]\t%*[^\n]", &address, bytes);
        str_trim(bytes, 400);

        // If we had triple dots last line we still have to fill in the data.
        if (triple_dot)
        {
            print_section(out, last_address, address, last_bytes, 400);
            triple_dot = false;
        }

        last_address = print_line_to_file(out, address, bytes);

        strncpy(last_bytes, bytes, 400);
        last_file_pos = ftell(in);
    }

    fseek(in, last_file_pos, SEEK_SET);
}
