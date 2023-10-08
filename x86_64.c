#include "x86_64.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <limits.h>

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
                if (!current->next) break;
                current = current->next;
                if (current->addr != address + (i+1))
                {
                    Run = 0;
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

    fprintf(fp, "%llX\t%llX\t%llX\t%llX\t%llX\t%llX\t%llX\t%llX\t%llX\t%llX\t%llX\t",
                  RIP,  RAX,  RBX,  RCX,  RDX,  RDI,  RSI,  RSP,  RBP,  R8,   R9);
    fprintf(fp, "%llX\t%llX\t%llX\t%llX\t%llX\t%llX\t%X\t%X\t%X\t%X\t",
                  R10,  R11,  R12,  R13,  R14,  R15, CF, OF, SF, ZF);

    fprintf(fp, "%llX", (RSP - RSPinit));
    fprintf(fp, "(\t");

    for (int i = RSPinit - 1; i >= RSP; i--)
    {
        fprintf(fp, "%X\t", ReadMem(i, 1));
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

    /* --- SHIFT AND ROTATE INSTRUCTIONS --- */

    if(!strcmp(Mnemonic, "rol")) rol();
    if(!strcmp(Mnemonic, "ror")) ror();
    if(!strcmp(Mnemonic, "sal")) sal();
    if(!strcmp(Mnemonic, "sar")) sar();
    if(!strcmp(Mnemonic, "shl")) shl();
    if(!strcmp(Mnemonic, "shr")) shr();

    /* --- CARRY INSTRUCTIONS --- */
    
    if (!strcmp(Mnemonic, "adc")) adc();

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
    if (ZF == 0 && SF == OF) ALUout = 1;
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
void seto(){
    if (OF = 1) ALUout = 1;
        else ALUout = 0;
    
}
void setz(){
    if (ZF == 1) ALUout = 1;
        else ALUout = 0;
}

/* --- SHIFT AND ROTATE INSTRUCTIONS --- */

void rol(){ //100% tested
    int mask;
    if (OpMaskSource == Mask8l) { 
        mask = 7;
        char val = (char)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            ALUout = (val << 1) | ((unsigned char)val >> mask);
            CF = (ALUout >> (mask + 1)) & 1;
            if (ALUin2 == 1 && i == 0) { OF = CF ^ ((ALUout >> mask) & 1); }
            if (CF == 1) ALUout |= 1;
            else ALUout &= ~(1);
            val = ALUout;
        }
        ALUout = ALUout & ~(Mask64-Mask8l);
    }
    //
    else if (OpMaskSource == Mask16) {
        mask = 15;
        short int val = (short int)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            ALUout = (val << 1) | ((unsigned short int)val >> mask);
            CF = (ALUout >> (mask + 1)) & 1;
            if (ALUin2 == 1 && i == 0) { OF = CF ^ ((ALUout >> mask) & 1); }
            if (CF == 1) ALUout |= 1;
            else ALUout &= ~(1);
            val = ALUout;
        }
        ALUout = ALUout & ~(Mask64-Mask16);
    }    
    //
    else if (OpMaskSource == Mask32) { 
        mask = 31;
        long long int val = ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            ALUout = (val << 1) | ((unsigned int)val >> mask);
            CF = (ALUout >> (mask + 1)) & 1;
            if (ALUin2 == 1 && i == 0) { OF = CF ^ ((ALUout >> mask) & 1); }
            if (CF == 1) ALUout |= 1;
            else ALUout &= ~(1);
            val = ALUout;
        }
        ALUout = ALUout & ~(Mask64-Mask32);
    }
    //
    else if (OpMaskSource == Mask64) { 
        mask = 63;
        for (int i = 0; i < ALUin2; i++)
        {
            ALUout = (ALUin1 << 1) | ((unsigned long long int)ALUin1 >> mask);
            CF = (ALUout >> (mask + 1)) & 1;
            if (ALUin2 == 1 && i == 0) { OF = CF ^ ((ALUout >> mask) & 1); }
            if (CF == 1) ALUout |= 1;
            else ALUout &= ~(1);
            ALUin1 = ALUout;
        }
    }
    else {}
}
void ror(){ //100% tested
    int mask;
    if (OpMaskSource == Mask8l) {
        mask = 7;
        char val = (char)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            ALUout = (val << mask) | ((unsigned char)val >> 1);
            CF = (ALUout >> mask) & 1;
            if (ALUin2 == 1 && i == 0) { OF = ((ALUout >> mask) & 1) ^ ((ALUout >> (mask - 1)) & 1); }
            if (CF == 1) ALUout |= (1<<mask);
            else ALUout &= ~(1<<mask);
            val = ALUout;
        }
        ALUout = ALUout & Mask8l;
    }
    //
    else if (OpMaskSource == Mask16) {
        mask = 15;
        short int val = (short int)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            ALUout = (val << mask) | ((unsigned short int)val >> 1);
            CF = (ALUout >> mask) & 1;
            if (ALUin2 == 1 && i == 0) { OF = ((ALUout >> mask) & 1) ^ ((ALUout >> (mask - 1)) & 1); }  
            if (CF == 1) ALUout |= (1<<mask);
            else ALUout &= ~(1<<mask);
            val = ALUout;
        }
        ALUout = ALUout & Mask16;
    }
    //
    else if (OpMaskSource == Mask32) {
        mask = 31;
        int val = (int)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            ALUout = (val << mask) | ((unsigned int)val >> 1);
            CF = (ALUout >> mask) & 1;
            if (ALUin2 == 1 && i == 0) { OF = ((ALUout >> mask) & 1) ^ ((ALUout >> (mask - 1)) & 1); }
            if (CF == 1) ALUout |= (1<<mask);
            else ALUout &= ~(1<<mask);
            val = ALUout;
        }
        ALUout = ALUout & Mask32;
    }
    //
    else if (OpMaskSource == Mask64) {
        mask = 63;
        for (int i = 0; i < ALUin2; i++)
        {
            ALUout = (ALUin1 << mask) | ((unsigned long long int)ALUin1 >> 1);
            CF = (ALUout >> mask) & 1;
            if (ALUin2 == 1 && i == 0) { OF = ((ALUout >> mask) & 1) ^ ((ALUout >> (mask - 1)) & 1); }    
            if (CF == 1) ALUout |= ((long long int)1<<mask);
            else ALUout &= ~((long long int)1<<mask);
            ALUin1 = ALUout;
        }        
    }
    else {}
}
void rcl(){ //100% tested
    int mask;
    if (OpMaskSource == Mask8l) { 
        mask = 7;
        int temp;
        char val = (char)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            temp = CF;
            ALUout = (val << 1) | ((unsigned char)val >> mask);
            CF = (ALUout >> (mask + 1)) & 1;
            if (temp == 1) ALUout |= 1;
            else ALUout &= ~(1);
            val = ALUout;
        }
        ALUout = ALUout & ~(Mask64-Mask8l);
        OF = CF ^ ((ALUout >> mask) & 1);
    }
    //
    else if (OpMaskSource == Mask16) {
        mask = 15;
        int temp; 
        short int val = (short int)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            temp = CF;
            ALUout = (val << 1) | ((unsigned short int)val >> mask);
            CF = (ALUout >> (mask + 1)) & 1;
            if (temp == 1) ALUout |= 1;
            else ALUout &= ~(1);
            val = ALUout;
        }
        ALUout = ALUout & ~(Mask64-Mask16);
        OF = CF ^ ((ALUout >> mask) & 1);
    }
    //
    else if (OpMaskSource == Mask32) {
        mask = 31;
        int temp;
        long long int val = ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            temp = CF;
            ALUout = (val << 1) | ((unsigned int)val >> mask);
            CF = (ALUout >> (mask + 1)) & 1;
            if (temp == 1) ALUout |= 1;
            else ALUout &= ~(1);
            val = ALUout;
        }
        ALUout = ALUout & ~(Mask64-Mask32);
        OF = CF ^ ((ALUout >> mask) & 1);
    }
    //
    else if (OpMaskSource == Mask64)
    {
        mask = 63;
        int temp;
        for (size_t i = 0; i < ALUin2; i++)
        {
            temp = CF;
            ALUout = (ALUin1 << 1) | ((unsigned long long int)ALUin1 >> mask);
            CF = (ALUout >> (mask + 1)) & 1;
            if (temp == 1) ALUout |= 1;
            else ALUout &= ~(1);
            ALUin1 = ALUout;
        }
        OF = CF ^ ((ALUout >> mask) & 1);
    }
    else {}
}
void rcr(){ //100% tested
    int mask;
    if (OpMaskSource == Mask8l) {
        mask = 7;
        int temp;
        char val = (char)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            temp = CF;
            ALUout = (val << mask) | ((unsigned char)val >> 1);
            CF = (ALUout >> mask) & 1;
            if (temp == 1) ALUout |= (1<<mask);
            else ALUout &= ~(1<<mask);
            val = ALUout;
        }
        ALUout = ALUout & Mask8l;
        OF = ((ALUout >> mask) & 1) ^ ((ALUout >> (mask - 1)) & 1);
    }
    //
    else if (OpMaskSource == Mask16) {
        mask = 15;
        int temp;
        short int val = (short int)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            temp = CF;
            ALUout = (val << mask) | ((unsigned short int)val >> 1);
            CF = (ALUout >> mask) & 1;
            if (temp == 1) ALUout |= (1<<mask);
            else ALUout &= ~(1<<mask);
            val = ALUout;
        }
        ALUout = ALUout & Mask16;
        OF = ((ALUout >> mask) & 1) ^ ((ALUout >> (mask - 1)) & 1);
    }
    //
    else if (OpMaskSource == Mask32) {
        mask = 31;
        int temp;
        int val = (int)ALUin1;
        for (size_t i = 0; i < ALUin2; i++)
        {
            temp = CF;
            ALUout = (val << mask) | ((unsigned int)val >> 1);
            CF = (ALUout >> mask) & 1;
            if (temp == 1) ALUout |= (1<<mask);
            else ALUout &= ~(1<<mask);
            val = ALUout;
        }
        ALUout = ALUout & Mask32;
        OF = ((ALUout >> mask) & 1) ^ ((ALUout >> (mask - 1)) & 1);
    }
    //
    else if (OpMaskSource == Mask64) {
        mask = 63;
        int temp;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            ALUout = (ALUin1 << mask) | ((unsigned long long int)ALUin1 >> 1);
            CF = (ALUout >> mask) & 1;
            if (temp == 1) ALUout |= ((long long int)1<<mask);
            else ALUout &= ~((long long int)1<<mask);
            ALUin1 = ALUout;
        }
        OF = ((ALUout >> mask) & 1) ^ ((ALUout >> (mask - 1)) & 1);
    }
    else {}
}
void sal(){ //100% tested
    int mask;
    if (OpMaskSource == Mask8l) { 
        mask = 7;
        char val = (char)ALUin1;
        int bitState = (val >> (mask - (ALUin2 - 1))) & 1;
        ALUout = (unsigned char)(val << ALUin2 & ~(Mask64 - Mask8l));
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { 
            if (((ALUout >> mask) & 1) == CF) OF = 0;
            else OF = 1; }
    }
    //
    else if (OpMaskSource == Mask16) { 
        mask = 15;
        short int val = (short int)ALUin1;
        int bitState = (val >> (mask - (ALUin2 - 1))) & 1;
        ALUout = (unsigned short int)(val << ALUin2 & ~(Mask64 - Mask16));
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { 
            if (((ALUout >> mask) & 1) == CF) OF = 0;
            else OF = 1; }
    }
    //
    else if (OpMaskSource == Mask32) { 
        mask = 31;
        int val = ALUin1;
        int bitState = (val >> (mask - (ALUin2 - 1))) & 1;
        ALUout = (unsigned int)(val << ALUin2 & ~(Mask64 - Mask32));
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { 
            if (((ALUout >> mask) & 1) == CF) OF = 0;
            else OF = 1; }
    }
    //
    else if (OpMaskSource == Mask64) {
        mask = 63;
        int bitState = (ALUin1 >> (mask - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { 
            if (((ALUout >> mask) & 1) == CF) OF = 0;
            else OF = 1; }
    }
    else {}
}
void sar(){ //100% tested
    int mask;
    if (OpMaskSource == Mask8l) {
        mask = 7;
        char val = (char)ALUin1;
        int bitState = (val >> (ALUin2 - 1)) & 1;
        ALUout = (char)(val >> ALUin2);
        ALUout = ALUout & Mask8l;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = 0; }
    }
    //
    else if (OpMaskSource == Mask16) { 
        mask = 15;
        short int val = (short int)ALUin1;
        int bitState = (val >> (ALUin2 - 1)) & 1;
        ALUout = (short int)(val >> ALUin2);
        ALUout = ALUout & Mask16;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = 0; }
    }
    //
    else if (OpMaskSource == Mask32) { 
        mask = 31;
        int val = (int)ALUin1;
        int bitState = (val >> (ALUin2 - 1)) & 1;
        ALUout = (int)(val >> ALUin2);
        ALUout = ALUout & Mask32;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = 0; }
    }
    //
    else if (OpMaskSource == Mask64) { 
        mask = 63;
        long long int val = (long long int)ALUin1;
        int bitState = (val >> (ALUin2 - 1)) & 1;
        ALUout = val >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = 0; }
    }
    else {}
}
void shl(){ //100% tested
    int mask;
    if (OpMaskSource == Mask8l) { 
        mask = 7;
        char val = (char)ALUin1;
        int bitState = (val >> (mask - (ALUin2 - 1))) & 1;
        ALUout = (unsigned char)(val << ALUin2 & ~(Mask64 - Mask8l));
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { 
            if (((ALUout >> mask) & 1) == CF) OF = 0;
            else OF = 1; }
    }
    //
    else if (OpMaskSource == Mask16) { 
        mask = 15;
        short int val = (short int)ALUin1;
        int bitState = (val >> (mask - (ALUin2 - 1))) & 1;
        ALUout = (unsigned short int)(val << ALUin2 & ~(Mask64 - Mask16));
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { 
            if (((ALUout >> mask) & 1) == CF) OF = 0;
            else OF = 1; }
    }
    //
    else if (OpMaskSource == Mask32) { 
        mask = 31;
        int val = ALUin1;
        int bitState = (val >> (mask - (ALUin2 - 1))) & 1;
        ALUout = (unsigned int)(val << ALUin2 & ~(Mask64 - Mask32));
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { 
            if (((ALUout >> mask) & 1) == CF) OF = 0;
            else OF = 1; }
    }
    //
    else if (OpMaskSource == Mask64) {
        mask = 63;
        int bitState = (ALUin1 >> (mask - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { 
            if (((ALUout >> mask) & 1) == CF) OF = 0;
            else OF = 1; }
    }
    else {}
}
void shr(){ //100% tested
    int mask;
    if (OpMaskSource == Mask8l) { 
        mask = 7;
        unsigned char val = (unsigned char)ALUin1;
        int bitState = (val >> (ALUin2 - 1)) & 1;
        ALUout = val >> ALUin2;
        ALUout = ALUout & Mask8l;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = (ALUin1 >> mask) & 1; }
    }
    //
    else if (OpMaskSource == Mask16) { 
        mask = 15;
        unsigned short int val = (unsigned short int)ALUin1;
        int bitState = (val >> (ALUin2 - 1)) & 1;
        ALUout = val >> ALUin2;
        ALUout = ALUout & Mask16;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = (ALUin1 >> mask) & 1; }
    }
    //
    else if (OpMaskSource == Mask32) { 
        mask = 31;
        unsigned int val = (unsigned int)ALUin1;
        int bitState = (val >> (ALUin2 - 1)) & 1;
        ALUout = val >> ALUin2;
        ALUout = ALUout & Mask32;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = (ALUin1 >> mask) & 1; }
    }
    //
    else if (OpMaskSource == Mask64) {
        mask = 63;
        int bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = (unsigned long long int)ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = (ALUin1 >> mask) & 1; }
    }
    else {}
}

/* --- CARRY INSTRUCTIONS --- */

void adc(){
    int mask;
    if (OpMaskSource == Mask8l)
    {
        mask = 7;
        char temp1 = (char)ALUin1;
        char temp2 = (char)ALUin2;
        char result = temp1 + temp2 + CF;
        if ((result >> mask) & 1) SF = 1;
        else SF = 0;
        if (result == 0) ZF = 1;
        else ZF = 0;
        temp2 += CF;
        //unsigned
        if ((unsigned char)temp1 > Mask8l - (unsigned char)temp2) CF = 1;
        else CF = 0;
        //signed
        if (temp1 < 0 == temp2 < 0 && (char)(temp1 + temp2) < 0 != temp1 < 0) OF = 1;
        else OF = 0;
        ALUout = result;
    }
    //
    else if (OpMaskSource == Mask16)
    {
        mask = 15;
        short int temp1 = (short int)ALUin1;
        short int temp2 = (short int)ALUin2;
        short int result = temp1 + temp2 + CF;
        if ((result >> mask) & 1) SF = 1;
        else SF = 0;
        if (result == 0) ZF = 1;
        else ZF = 0;
        temp2 += CF;
        //unsigned
        if ((unsigned short int)temp1 > Mask16 - (unsigned short int)temp2) CF = 1;
        else CF = 0;
        //signed
        if (temp1 < 0 == temp2 < 0 && (short int)(temp1 + temp2) < 0 != temp1 < 0) OF = 1;
        else OF = 0;
        ALUout = result;
    }
    //
    else if (OpMaskSource == 32)
    {
        mask = 31;
        int temp1 = (int)ALUin1;
        int temp2 = (int)ALUin2;
        int result = temp1 + temp2 + CF;
        if ((result >> mask) & 1) SF = 1;
        else SF = 0;
        if (result == 0) ZF = 1;
        else ZF = 0;
        temp2 += CF;
        //unsigned
        if ((unsigned int)temp1 > Mask32 - (unsigned int)temp2) CF = 1;
        else CF = 0;
        //signed
        if (temp1 < 0 == temp2 < 0 && (int)(temp1 + temp2) < 0 != temp1 < 0) OF = 1;
        else OF = 0;
        ALUout = result;
    }
    else if (OpMaskSource == 64)
    {
        mask = 7;
        long long int temp1 = ALUin1;
        long long int temp2 = ALUin2;
        long long int result = temp1 + temp2 + CF;
        if ((result >> mask) & 1) SF = 1;
        else SF = 0;
        if (result == 0) ZF = 1;
        else ZF = 0;
        temp2 += CF;
        //unsigned
        if ((unsigned long long int)temp1 > Mask64 - (unsigned long long int)temp2) CF = 1;
        else CF = 0;
        //signed
        if (temp1 < 0 == temp2 < 0 && (long long int)(temp1 + temp2) < 0 != temp1 < 0) OF = 1;
        else OF = 0;
        ALUout = result;
    }
    else {}    
}
