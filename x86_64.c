//INCLUDES
#include "x86_64.h"

#include <stdio.h>
#include <stdlib.h>

long long int ReadMem(long long int address, int size)
{
    //Data memory
    MemDataUnit * current = MemData;
    long long int toreturn = 0;
    while (current != NULL) {
        if (current->addr == address)
        {
            for (int i = 0; i < size; i++)
            {                
                toreturn |= (long long int)(((current->byte)>>(i*8))&0xFF);
                current = current->next;
                if (current->addr != address+(i+1)) //Non-following address problem
                {
                    Run = 0; //Global error variable; stop running
                    fprintf(stderr, "Memory read error! Not following address!");
                    exit(42);
                }                
            }
            return toreturn;
        }
        else
        {
            current = current->next;
        }        
    }
    fprintf(stderr, "Memory read error! Data not in memory!");    
    exit(42);
}

void WriteMem(long long int address, int size, long long int data)
{
    //Data memory
    int i;
    MemDataUnit * current = MemData;
    MemDataUnit * lower = NULL;
    MemDataUnit * temp;
    while (current != NULL && current->addr < address) {
        lower = current;
        current = current->next;
    }
    for (int i = 0; i < size; i++) {
        if(current != NULL && current->addr == address+i){
            current->byte = (unsigned char)((data>>(i*8))&0xff);
        }
        else{
            temp = (MemDataUnit*)malloc(sizeof(MemDataUnit));
            temp->addr = address+i;
            temp->byte = (unsigned char)((data>>(i*8))&0xff);
            temp->next = current;
            if(lower == NULL) MemData = temp;
            else lower->next = temp;
            current = temp;
        }
        lower = current;
        current = current->next;
    }
}

void SaveState()
{
    char* file = "output.txt";
    FILE *fp = fopen(file, "a");
    if (fp == NULL)
    {
        fprintf(stderr, "File error! Cannot open file!");
        exit(32);
    }

    fprintf(fp, "%lld \t %lld \t %lld \t %lld \t %lld \t %lld \t %lld \t %lld \t %lld \t %lld \t %lld \t ",
                 RIP,    RAX,    RBX,    RCX,    RDX,    RDI,    RSI,    RSP,    RBP,    R8,     R9);
    fprintf(fp, "%lld \t %lld \t %lld \t %lld \t %lld \t %lld \t %d \t %d \t %d \t %d \t ",
                 R10,    R11,    R12,    R13,    R14,    R15,    CF,   OF,   SF,   ZF);

    fprintf(fp, "<%lld> ", (RSP - RBP));
    fprintf(fp, "(\t <StackBytes_InReverseOrder>) \t ");//todo
    fprintf(fp, "<%s> \n", Mnemonic);

    fclose(fp);
}
