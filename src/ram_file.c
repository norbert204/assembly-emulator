#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "ram_file.h"
#include "string_utils.h"

#define OUTPUT_FILE_NAME "RAM.txt"
#define LINE_SIZE 255

#define MAIN_FUNCTION_NAME "<main>"

char *prefixes[] = {
    "cs",
    "ss",
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

typedef enum 
{
    NO_SEGMENT = 0,
    TEXT,
    RODATA,
    DATA,
    BSS,
} segment;

void set_state(char *line, segment *seg, FILE **out)
{
    if (strstr(line, ".text"))
    {
        *seg = TEXT;
        fprintf(*out, ".text\n");
    }
    else 
    {
        *seg = NO_SEGMENT;
    }
}

int get_main_address(FILE **in)
{
    char line[LINE_SIZE];

    while (!strstr(line, MAIN_FUNCTION_NAME))
    {
        fgets(line, LINE_SIZE, *in);
    }

    int address;
    sscanf(line, "%x", &address);

    fseek(*in, 0, SEEK_SET);

    return address;
}

char *get_original_assembly(char *line)
{
    char *instruction_token = strtok(line, " ");

    bool has_prefix = true;

    while (has_prefix)
    {
        bool found = false;
        for (int i = 0; i < PREFIXES_COUNT; i++)
        {
            if (!strcmp(prefixes[i], instruction_token))
            {
                found = true;
            }
        }

        if (found)
        {
            instruction_token = strtok(NULL, " ");
        }

        has_prefix = found;
    }

    char *mnemonic = calloc(16, sizeof(char));
    char *args = calloc(128, sizeof(char));

    strcpy(mnemonic, instruction_token);
    str_trim(&mnemonic);

    instruction_token = strtok(NULL, " ");

    // Instruction has arguments
    if (instruction_token != NULL)
    {
        strcpy(args, instruction_token);

        while (instruction_token != NULL)
        {
            instruction_token = strtok(NULL, " ");
            if (instruction_token != NULL)
            {
                strcat(args, " ");
                strcat(args, instruction_token);
            }
        }

        // this could be a separate function

        int end = str_last_index_of(args, '#');
        if (end == -1)
        {
            end = str_last_index_of(args, '<');
        }

        if (end != -1)
        {
            char tmp[128];
            strcpy(tmp, args);

            memset(args, '\0', strlen(args) + 1);
            strncpy(args, tmp, end);
        }

        str_trim(&args);

        char *arg = strtok(args, ",");
        char *cleaned_args = calloc(128, sizeof(char));

        while (arg != NULL)
        {
            if (strstr(arg, "PTR") && strstr(arg, ":"))
            {
                char *colon_location = strchr(arg, ':');
                char *begining = calloc(32, sizeof(char));
                char *end = calloc(32, sizeof(char));

                int colon_index = (int)(colon_location - arg);
                strncpy(begining, arg, colon_index - 3);
                strcpy(end, arg + colon_index + 1);

                char cleaned_arg[128];
                sprintf(cleaned_arg, "%s %s", begining, end);

                if (strlen(cleaned_args) == 0)
                {
                    strcat(cleaned_args, cleaned_arg);
                }
                else 
                {
                    strcat(cleaned_args, ",");
                    strcat(cleaned_args, cleaned_arg);
                }

                free(begining);
                free(end);
            }
            else
            {
                if (strlen(cleaned_args) == 0)
                {
                    strcat(cleaned_args, arg);
                }
                else 
                {
                    strcat(cleaned_args, ",");
                    strcat(cleaned_args, arg);
                }
            }
            
            arg = strtok(NULL, ",");
        }

        memset(args, '\0', 128);
        strcpy(args, cleaned_args);
    }

    char *original_assembly = calloc(16 + 128, sizeof(char));

    sprintf(original_assembly, "%s\t%s", mnemonic, args);

    free(mnemonic);
    free(args);

    return original_assembly;
}

char *get_simplified_assembly(char *assembly)
{
    char *result = calloc(256, sizeof(char));
    int arg_count = 0;

    int tab_index = str_index_of(assembly, '\t');

    if (tab_index != -1)
    {
        char *mnemonic = calloc(32, sizeof(char));
        strncpy(mnemonic, assembly, tab_index);
        sprintf(result, "%s\t", mnemonic);

        char *args = calloc(128, sizeof(char));
        strcpy(args, assembly + tab_index + 1);

        char *saved;
        for (char *token = strtok_r(args, ",", &saved); token && *token; token = strtok_r(NULL, ",", &saved))
        {
            arg_count++;

            if (strstr(token, "PTR"))
            {
                int last_space_index = str_last_index_of(token, ' '); 
                char *instruction = calloc(64, sizeof(char));
                char *address = calloc(48, sizeof(char));
                strcpy(address, token + last_space_index + 1);
                sprintf(instruction, "%c%s", token[0], address);

                strcat(result, instruction);

                free(address);
                free(instruction);
            }
            else 
            {
                strcat(result, token);
            }
            strcat(result, "\t");
        }

        free(args);
        free(mnemonic);
    }
    else 
    {
        sprintf(result, "%s\t", assembly);
    }
    
    for (int i = arg_count; i < 3; i++)
    {
        strcat(result, "_\t");
    }

    return result;
}

void cleanup_dump(char *file)
{
    FILE *in = fopen(file, "r");
    FILE *out = fopen(OUTPUT_FILE_NAME, "w");

    char line[LINE_SIZE];

    segment cur_segment = NO_SEGMENT;

    // First we need to find the address of the main function
    int address = get_main_address(&in);
    fprintf(out, "main: %x\n", address);

    bool found_main = false;

    // After finding the the address we can start iterating through the file.
    while (fgets(line, LINE_SIZE, in) != NULL)
    {
        // If we find a line that starts with "Disassembly of", we need to check for a new state
        if (str_starts_with(line, "Disassembly of"))
        {
            set_state(line, &cur_segment, &out);
            continue;
        }

        switch (cur_segment)
        {
            case TEXT:
                if (!found_main)
                {
                    if (strstr(line, MAIN_FUNCTION_NAME))
                    {
                        found_main = true;
                    }       
                    break;
                }

                if (str_starts_with(line, "  "))
                {
                    char *token = strtok(line, "\t");
                    
                    char *address = calloc(64, sizeof(char));
                    strcpy(address, token);
                    str_trim(&address);

                    address[strlen(address) - 1] = '\0';

                    token = strtok(NULL, "\t");

                    char *machine_code = calloc(128, sizeof(char));
                    strcpy(machine_code, token);

                    str_trim(&machine_code);
                    str_remove_char(&machine_code, ' ');

                    if (str_contains_only(machine_code, '0'))
                    {
                        // Here we have to concatenate the current machine code to the previous.                        
                    }
                    else
                    {
                        token = strtok(NULL, "\t");
                        
                        if (token != NULL)
                        {
                            char *original_assembly = get_original_assembly(token);
                            char *simplified_assembly = get_simplified_assembly(original_assembly);
                            fprintf(out, "%s\t%s\t%s\t%s\n", address, simplified_assembly, machine_code, original_assembly);

                            free(original_assembly);
                            free(simplified_assembly);
                        }
                    }

                    free(address);
                    free(machine_code);
                }
                break;
        }
    }

    fclose(in);
    fclose(out);
}
