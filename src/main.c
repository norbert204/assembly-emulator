#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ram_file/compile.h"
#include "ram_file/decompile.h"
#include "emulator/x86_64.h"
#include "ram_file/ram_file.h"

#define OUTPUT_DUMP_FILE "/tmp/asemu_dump"
#define OUTPUT_RAM_FILE "/tmp/asemu_ram"
#define OUTPUT_EXECUTABLE "/tmp/asemu_executable"

int enable_debugger = 0;

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
    puts("\tasemu [parameters] <file>");
    puts("");
    puts("Optional parameters");
    puts("\t-h\tPrint this help text");
    puts("\t-c\tPrint credits");
    puts("\t-e\tThe provided file is a precompiled executable");
    puts("\t-r\tThe provided file is a premade ram file");
}

void print_file_specifier_error()
{
    fprintf(stderr, "Provide only one file type specifying parameter!\n");
    exit(1);
}

void debugger() {

    char* file2 = "/tmp/asemu_stack";
    FILE *fp2 = fopen(file2, "w");
    if (fp2 == NULL)
    {
        fprintf(stderr, "File error! Cannot open file!");
        exit(32);
    }
    fprintf(fp2, "size: %d\n", RSPinit - RSP);
    for (int i = 0; i <= RSPinit - RSP; i++)
    {  
        if(RSPinit-i == RBP){
            fprintf(fp2, "RBP\t%x\n", ReadMem(RSPinit-i, 1));
            continue;
        }
        fprintf(fp2, "%llx\t%x\n",RSPinit-i, ReadMem(RSPinit-i, 1));
        
    }
        fclose(fp2);


    char tmp = 0;
    printf("Debugger(type \x1b[34mh\x1b[0m for help): ");

    tmp = getchar();
    while (tmp != '\n' && getchar() != '\n');

    if (tmp != 'n' && tmp != 'm' && tmp != 'r' && tmp != 'h') {
        exit(32);
    }
    if (tmp == 'h'){
        printf("\t\t\x1b[34m##########\x1b[0m\n\tNext instruction: n\n\tPrint memory: m\n\tRegister value: r\n\tStop the program: any other key\n\t\t\x1b[34m##########\x1b[0m\n");
        debugger();
    }
    if (tmp == 'm') {
        PrintList("data");
        debugger();
    }
    if (tmp == 'r') {
        long long int mask, dest;
        char buffer[10];
    printf("\x1b[31mEnter a register name: \x1b[0m");
    fgets(buffer, sizeof(buffer), stdin);
    size_t length = strlen(buffer);
    if (length > 0 && buffer[length - 1] == '\n') {
        buffer[length - 1] = '\0';
    }

    //printf("You entered: %s\n", buffer);
    if(FetchRegister(buffer, &dest, &mask)){
    printf("%s value: %llx\n", buffer, dest & mask);
        debugger();
    }
    else {
        puts("Your input is wrong");
        debugger();
    }
    }
}

void emulation()
{
    Init();

    while(Run)
    {
        SaveState();
        if(enable_debugger) puts("\x1b[32m-State saved");
        InstructionFetch();
        if (enable_debugger){
            puts("\x1b[32m-Instruction loaded\x1b[0m");
            printf("%llX | %s | %s | %s | length: %d\n", RIP-InstLength, Mnemonic, Operand1, Operand2,InstLength);
        }
        OperandFetch();
        if (enable_debugger){
            puts("\x1b[32m-Operands fetched\x1b[0m");
            printf("Operand1: \033[036m%llx\033[0m, Operand2: \033[036m%llx\033[0m\n", ALUin1, ALUin2);
        }
        
        Execute();
        if (enable_debugger){
            puts("\x1b[32m-Executed\x1b[0m");
            printf("ALUout: %llx\n", ALUout);
        }
        
        WriteBack();
        if (enable_debugger){
            puts("\x1b[32m-Result saved\x1b[0m");
            printf("Address: %llx\n", WriteBackAddress);
        }
        
        if (enable_debugger){
            debugger();
        }
    }

    Fini();
}

void print_information_for_gui()
{
    puts("asemu");
}

int main(int argc, char *argv[])
{

    if (argc == 1)
    {
        print_help();
        exit(1);
    }

    int run_state = 0;

    // Handle the optional parameters.
    for (int i = 0; i < argc; i++)
    {
        if (!strcmp(argv[i], "-c"))
        {
            print_credits();
            exit(0);
        }

        if (!strcmp(argv[i], "-h"))
        {
            print_help();
            exit(0);
        }
        if (!strcmp(argv[i], "-d"))
        {
            enable_debugger = 1;
        }

        if (!strcmp(argv[i], "--gui-info"))
        {
            print_information_for_gui();
            exit(0);
        }

        if (!strcmp(argv[i], "-e"))
        {
            if (run_state != 0)
            {
                print_file_specifier_error();
            }

            run_state = 1;
            continue;
        }

        if (!strcmp(argv[i], "-r"))
        {
            if (run_state != 0)
            {
                print_file_specifier_error();
            }

            run_state = 2;
            continue;
        }
    }

    char *file = argv[argc - 1];

    if (run_state == 0)
    {
        compile_code(file, OUTPUT_EXECUTABLE);
        decompile_executable(OUTPUT_EXECUTABLE, OUTPUT_DUMP_FILE);
        create_ram_file(OUTPUT_DUMP_FILE, OUTPUT_RAM_FILE);
        emulation();
    }
    else if (run_state == 1)
    {
        decompile_executable(file, OUTPUT_DUMP_FILE);
        create_ram_file(OUTPUT_DUMP_FILE, OUTPUT_RAM_FILE);
        emulation();
    }
    else if (run_state == 2)
    {
        // TODO: Make the emulator have an input file parameter.
        char command[256];
        sprintf(command, "cp %s /tmp/asemu_ram", file);
        system(command);

        emulation();
    }

    return 0;
}
