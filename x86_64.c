//INCLUDES
#include "x86_64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

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
                toreturn |= (long long int)(((current->byte)<<(i*8))&0xFFFFFFFF);
                current = current->next;
                if (current->addr != address+(i+i)) //Non-following address problem
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

    fprintf(fp, "<%lld> ", (RSP - RSPinit));
    fprintf(fp, "(\t <");

    for (int i = RSPinit - 1; i >= RSP; i--)
    {
        fprintf(fp, "%d\t", ReadMem(i, 1));//todo    
    }
    
    fprintf(fp, ")>) \t ");
    fprintf(fp, "<%s> \n", Mnemonic);

    fclose(fp);
}

void ProcadureForAllMnemoninc()
{
    if (!strcmp(Mnemonic, "ja"))
    {
        if (ZF == 0 && CF == 0) // (ZF == 0 && CF == 1)
        {
            RIP = ALUin1;    
        }   
        CF = 0;
        ZF = 0;
        return;      
    }
    if (!strcmp(Mnemonic, "jae"))
    {
        if (CF == 0) // ((ZF == 0 && CF == 0) || ZF == 1)
        {
            RIP = ALUin1;
        }   
        CF = 0;
        return;
    }
    if (!strcmp(Mnemonic, "jb"))
    {
        if (CF == 1) // (ZF == 0 && CF == 0)
        {
            RIP = ALUin1;
        }   
        CF = 1;
        ZF = 0;
        return;
    }
    if (!strcmp(Mnemonic, "jbe"))
    {
        if (CF == 1 || ZF == 1) // ((ZF == 0 && CF == 1) || ZF == 1)
        {
            RIP = ALUin1; 
        }   
        CF = 1;
        ZF = 1; 
        return;
    }
    if (!strcmp(Mnemonic, "jc"))
    {
        if (CF == 1)
        {
            RIP = ALUin1;
        }
        return;
    }
    if (!strcmp(Mnemonic, "je"))
    {
        if (ZF == 1)
        {
            RIP = ALUin1;
        }   
        ZF = 1;
        return;
    }
    if (!strcmp(Mnemonic, "jg"))
    {
        if (ZF == 0 && SF == OF)
        {
            RIP = ALUin1;
        }
        ZF = 0;
        SF = 1;
        return;
    }
    if (!strcmp(Mnemonic, "jge"))
    {
        if (SF == OF) // (ZF == 1 || SF == OF)
        {
            RIP = ALUin1;
        }
        ZF = 1;
        SF = 1;
        return;
    }
    if (!strcmp(Mnemonic, "jl"))
    {
        if (SF != OF)
        {
            RIP = ALUin1;
        }
        return;
    }
    if (!strcmp(Mnemonic, "jle"))
    {
        if (ZF == 1 || SF != OF)
        {
            RIP = ALUin1;
        }
        ZF = 1;
        return;
    }
    if (!strcmp(Mnemonic, "jmp"))
    {
        RIP = ALUin1;
        return; 
    }
    if (!strcmp(Mnemonic, "jnc"))
    {
        if (CF == 0)
        {
            RIP = ALUin1;
        }
        CF = 0;
        return;
    }
    if (!strcmp(Mnemonic, "jne"))
    {
        if (ZF == 0)
        {
            RIP = ALUin1;
        }
        ZF = 0;
        return;
    }
    if (!strcmp(Mnemonic, "jnz"))
    {
        if (ZF == 0)
        {
            RIP = ALUin1;
        }              
        ZF = 0;
        return;
    }
    if (!strcmp(Mnemonic, "jz"))
    {
        if (ZF == 1)
        {
            RIP = ALUin1;
        }              
        ZF = 1;
        return;
    }
}
