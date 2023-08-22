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
                toreturn |= (long long int)(((long long int)(current->byte)<<(i*8)));
                if(!current->next) break;
                current = current->next;
                if (current->addr != address+(i+i))
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

    fprintf(fp, "%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t",
                  RIP,  RAX,  RBX,  RCX,  RDX,  RDI,  RSI,  RSP,  RBP,  R8,   R9);
    fprintf(fp, "%lld\t%lld\t%lld\t%lld\t%lld\t%lld\t%d\t%d\t%d\t%d\t",
                  R10,  R11,  R12,  R13,  R14,  R15, CF, OF, SF, ZF);

    fprintf(fp, "%lld", (RSP - RSPinit));
    fprintf(fp, "(\t");

    for (int i = RSPinit - 1; i >= RSP; i--)
    {
        fprintf(fp, "%d\t", ReadMem(i, 1));
    }
    
    fprintf(fp, ")\t");
    fprintf(fp, "%s\n", Mnemonic);

    fclose(fp);
}

void Execute(){

    /* --- JUMP INSTRUCTIONS ---*/

    if (!strcmp(Mnemonic, "ja")) ja();
    if (!strcmp(Mnemonic, "jae")) jae();
    if (!strcmp(Mnemonic, "jb")) jb();
    if (!strcmp(Mnemonic, "jbe")) jbe();
    if (!strcmp(Mnemonic, "jc")) jc();
    if (!strcmp(Mnemonic, "je")) je();
    if (!strcmp(Mnemonic, "jg")) jg();
    if (!strcmp(Mnemonic, "jge")) jge();
    if (!strcmp(Mnemonic, "jl")) jl();
    if (!strcmp(Mnemonic, "jle")) jle();
    if (!strcmp(Mnemonic, "jmp")) jmp();
    if (!strcmp(Mnemonic, "jnc")) jnc();
    if (!strcmp(Mnemonic, "jne")) jne();
    if (!strcmp(Mnemonic, "jnz")) jnz();
    if (!strcmp(Mnemonic, "jz")) jz();

    /* --- SET INSTRUCTIONS --- */

    if (!strcmp(Mnemonic, "seta")) seta();
    if (!strcmp(Mnemonic, "setae")) setae();
    if (!strcmp(Mnemonic, "setb")) setb();
    if (!strcmp(Mnemonic, "setbe")) setbe();
    if (!strcmp(Mnemonic, "setc")) setc();
    if (!strcmp(Mnemonic, "sete")) sete();
    if (!strcmp(Mnemonic, "setg")) setg();
    if (!strcmp(Mnemonic, "setge")) setge();
    if (!strcmp(Mnemonic, "setl")) setl();
    if (!strcmp(Mnemonic, "setle")) setle();
    if (!strcmp(Mnemonic, "setnc")) setnc();
    if (!strcmp(Mnemonic, "setne")) setne();
    if (!strcmp(Mnemonic, "setnz")) setnz();
    if (!strcmp(Mnemonic, "setz")) setz();
}

/* --- JUMP INSTRUCTIONS ---*/

void ja(){
    if (ZF == 0 && CF == 0) RIP = ALUin1;
}
void jae(){
    if (CF == 0) RIP = ALUin1;
}
void jb(){
    if (CF == 1) RIP = ALUin1;
}
void jbe(){
    if (CF == 1 || ZF == 1) RIP = ALUin1;
}
void jc(){
    if (CF == 1) RIP = ALUin1;
}
void je(){
    if (ZF == 1) RIP = ALUin1;
}
void jg(){
    if (ZF == 0 && SF == OF) RIP = ALUin1;
}
void jge(){
    if (SF == OF) RIP = ALUin1;
}
void jl(){
    if (SF != OF) RIP = ALUin1;
}
void jle(){
    if (ZF == 1 || SF != OF) RIP = ALUin1;
}
void jmp(){
    RIP = ALUin1;
}
void jnc(){
    if (CF == 0) RIP = ALUin1;
}
void jne(){
    if (ZF == 0) RIP = ALUin1;
}
void jnz(){
    if (ZF == 0) RIP = ALUin1;
}
void jz(){
    if (ZF == 1) RIP = ALUin1;
}

/* --- SET INSTRUCTIONS --- */

void seta(){
    if (CF == 0 && ZF == 0) ALUout = 1;
        else ALUout = 0;
}
void setae(){
    if (CF == 0) ALUout = 1;
        else ALUout = 0;
}
void setb(){
    if (CF == 1) ALUout = 1;
        else ALUout = 0;
}
void setbe(){
    if (CF == 1 || ZF == 1) ALUout = 1;
        else ALUout = 0;
}
void setc(){
    if (CF == 1) ALUout = 1;
        else ALUout = 0;
}
void sete(){
    if (ZF == 1) ALUout = 1;
        else ALUout = 0;
}
void setg(){
    if (ZF == 0 and SF == OF) ALUout = 1;
        else ALUout = 0;
}
void setge(){
    if (SF == OF) ALUout = 1;
        else ALUout = 0;
}
void setl(){
    if (SF != OF) ALUout = 1;
        else ALUout = 0;
}
void setle(){
    if (ZF == 1 || SF != OF) ALUout = 1;
        else ALUout = 0;
}
void setnc(){
    if (CF == 0) ALUout = 1;
        else ALUout = 0;
}
void setne(){
    if (ZF == 0) ALUout = 1;
        else  ALUout = 0;
}
void setnz(){
    if (ZF == 0) ALUout = 1;
        else ALUout = 0;
}
void setz(){
    if (ZF == 1) ALUout = 1;
        else ALUout = 0;
}
