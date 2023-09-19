#include "segment_text.h"
#include <stdbool.h>
#include <string.h>
#include "../base/string_utils.h"

#define MAIN_FUNCTION_NAME " <main>:"

const char *prefixes[] = {
    "cs",
    "ss",
    "ds",
    "data16",
    "addr32",
    "bnd", 
    "lock",
    "rex.R",
    "rex.RB", 
    "rex.RX",
    "rex.RXB",
    "rex.XB",
    "rex.W",
    "rex.WB",
    "rex.WXB",
    "rex.WRX",
    "rex.WRXB",
};

#define PREFIXES_COUNT 16

static void print_line_to_file(FILE *out, int address, char machine_code[], char assembly[], char simplified_assembly[])
{
    fprintf(out, "%x\t%s\t%s\t%s\n", address, simplified_assembly, machine_code, assembly);
}

void clean_operand(char *operand, const size_t size)
{
    if (operand[0] == '\0')
    {
        return;
    }

    // Remove comments
    char commentless[size];
    memset(commentless, '\0', size);

    sscanf(operand, "%[^<#\n]", commentless);
    str_trim(commentless, size);

    strncpy(operand, commentless, size);

    // If operand is a pointer then we've to remove the prefix from the address.
    if (strstr(operand, "PTR"))
    {
        char type[16];
        char address[16];

        memset(type, '\0', 16);
        memset(address, '\0', 16);

        sscanf(operand, "%s PTR %s", type, address);

        int colon = str_index_of(address, ':');
        if (colon != -1)
        {
            strncpy(address, address + colon + 1, 16);
        }

        sprintf(operand, "%s PTR %s", type, address);
    }
}

void clean_assembly(char *assembly, const size_t size)
{
    // Remove the prefix from the mnemonic
    for (int i = 0; i < PREFIXES_COUNT; i++)
    {
        if (!str_starts_with(assembly, prefixes[i]))
        {
            continue;
        }

        int prefix_len = strlen(prefixes[i]);

        strncpy(assembly, assembly + prefix_len, size);

        break;
    }

    char mnemonic[32];
    char operand1[32];
    char operand2[32];
    char operand3[32];

    memset(mnemonic, '\0', 32);
    memset(operand1, '\0', 32);
    memset(operand2, '\0', 32);
    memset(operand3, '\0', 32);

    sscanf(assembly, "%31s%*[ \n]%31[^,\n]%*[,\n]%31[^,\n]%*[,\n]%31[^\n]", mnemonic, operand1, operand2, operand3);

    clean_operand(operand1, 32);
    clean_operand(operand2, 32);
    clean_operand(operand3, 32);

    sprintf(assembly, "%s", mnemonic);

    if (!str_empty(operand1))
    {
        strncat(assembly, " ", size);
        strncat(assembly, operand1, size);
    }
    if (!str_empty(operand2))
    {
        strncat(assembly, ",", size);
        strncat(assembly, operand2, size);
    }
    if (!str_empty(operand3))
    {
        strncat(assembly, ",", size);
        strncat(assembly, operand3, size);
    }
}

void simplify_operand(char operand[], size_t size)
{
    if (str_empty(operand))
    {
        operand[0] = '_';
        return;
    }

    if (!strstr(operand, "PTR"))
    {
        return;
    }

    // If the operand is a pointer type we have to simplify it.
    char type = operand[0];

    char address[size - 1];
    memset(address, '\0', size - 1);
    sscanf(operand, "%*[^ ] PTR %s", address);

    if (address[0] == '[')
    {
        sprintf(operand, "%c%s", type, address);
    }
    else
    {
        sprintf(operand, "%c[%s]", type, address);
    }
}

void simplify_assembly(char assembly[], size_t size)
{
    char mnemonic[32];
    char operand1[32];
    char operand2[32];
    char operand3[32];

    memset(mnemonic, '\0', 32);
    memset(operand1, '\0', 32);
    memset(operand2, '\0', 32);
    memset(operand3, '\0', 32);

    sscanf(assembly, "%s %[^,\n]%*[,\n]%[^,\n]%*[,\n]%[^,\n]", mnemonic, operand1, operand2, operand3);

    simplify_operand(operand1, 32);
    simplify_operand(operand2, 32);
    simplify_operand(operand3, 32);

    sprintf(assembly, "%s\t%s\t%s\t%s", mnemonic, operand1, operand2, operand3);
}

void clean_machine_code(char machine_code[], size_t size)
{
    str_trim(machine_code, size);
    str_remove_char(machine_code, size, ' ');
}

void handle_text_segment(FILE *input, FILE *output)
{
    bool read_values = false;

    char line[512];

    int address;
    char machine_code[128];
    char assembly[128];
    char simplified_assembly[128];

    memset(machine_code, '\0', 128);
    memset(assembly, '\0', 128);
    memset(simplified_assembly, '\0', 128);

    long last_file_pos = ftell(input);

    while (!feof(input))
    {
        fgets(line, 512, input);

        if (str_starts_with(line, "Disassembly of"))
        {
            fseek(input, last_file_pos, SEEK_SET);
            break;
        }

        // We have to append the machine code to the last line
        if (str_char_count(line, 512, '\t') == 1 && read_values)
        {
            char rest_of_machine_code[32];
            memset(rest_of_machine_code, '\0', 32);

            sscanf(line, "%*[^\t]\t%[^\n]", rest_of_machine_code);

            clean_machine_code(rest_of_machine_code, 32);

            strncat(machine_code, rest_of_machine_code, 32);

            print_line_to_file(output, address, machine_code, assembly, simplified_assembly);

            read_values = false;
            continue;
        }

        // Only print out the line if there were read values
        if (read_values)
        {
            print_line_to_file(output, address, machine_code, assembly, simplified_assembly);
            read_values = false;
        }

        // We don't have to read anything if the line is empty or just a label line
        if (str_char_count(line, 512, '\t') == 0)
        {
            read_values = false;
            continue;
        }

        sscanf(line, "%x:\t%[^\t\n]\t%[^\t\n]", &address, machine_code, assembly);

        clean_machine_code(machine_code, 128);
        clean_assembly(assembly, 128);

        strncpy(simplified_assembly, assembly, 128);
        simplify_assembly(simplified_assembly, 128);

        last_file_pos = ftell(input);
        read_values = true;
    }
}
