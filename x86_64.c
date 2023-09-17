//INCLUDES
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

void rol(){
    if (OpMaskDest == Mask8l)
    {
        ALUout = (ALUin1 << ALUin2) | (ALUin1 >> (8 - ALUin2));
        CF = ALUin1 >> (7 - (ALUin2 - 1)) & 1;
        if(ALUin2 == 1) { OF = CF ^ ((ALUout >> 7) & 1); }
    }
    if (OpMaskDest == Mask16)
    {
        ALUout = (ALUin1 << ALUin2) | (ALUin1 >> (16 - ALUin2));
        CF = ALUin1 >> (15 - (ALUin2 - 1)) & 1;
        if(ALUin2 == 1) { OF = CF ^ ((ALUout >> 15) & 1); }
    }
    if (OpMaskDest == Mask32)
    {
        ALUout = (ALUin1 << ALUin2) | (ALUin1 >> (32 - ALUin2));
        CF = ALUin1 >> (31 - (ALUin2 - 1)) & 1;
        if(ALUin2 == 1) { OF = CF ^ ((ALUout >> 31) & 1); }
    }
    if (OpMaskDest == Mask64)
    {        
        ALUout = (ALUin1 << ALUin2) | (ALUin1 >> (64 - ALUin2));
        CF = ALUin1 >> (63 - (ALUin2 - 1)) & 1;
        if(ALUin2 == 1) { OF = CF ^ ((ALUout >> 63) & 1); }
    }
}
void ror(){
    if (OpMaskDest == Mask8l)
    {
        ALUout = (ALUin1 >> ALUin2) | (ALUin1 << (8 - ALUin2));
        CF = ALUin1 >> (ALUin2 - 1) & 1;
        if (ALUin2 == 1) { OF = ((ALUout >> 7) & 1) ^ ((ALUout >> 6) & 1); }
    }
    if (OpMaskDest == Mask16)
    {
        ALUout = (ALUin1 >> ALUin2) | (ALUin1 << (16 - ALUin2));
        CF = ALUin1 >> (ALUin2 - 1) & 1;
        if (ALUin2 == 1) { OF = ((ALUout >> 15) & 1) ^ ((ALUout >> 14) & 1); }
    }
    if (OpMaskDest == Mask32)
    {
        ALUout = (ALUin1 >> ALUin2) | (ALUin1 << (32 - ALUin2));
        CF = ALUin1 >> (ALUin2 - 1) & 1;
        if (ALUin2 == 1) { OF = ((ALUout >> 31) & 1) ^ ((ALUout >> 30) & 1); }
    }
    if (OpMaskDest == Mask64)
    {
        ALUout = (ALUin1 >> ALUin2) | (ALUin1 << (64 - ALUin2));
        CF = ALUin1 >> (ALUin2 - 1) & 1;
        if (ALUin2 == 1) { OF = ((ALUout >> 63) & 1) ^ ((ALUout >> 62) & 1); }
    }
}
void rcl(){
    if (OpMaskDest == Mask8l)
    {
        int temp = 0;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            CF = ALUin1 >> 7 & 1;
            ALUout = (ALUin1 << 1) | (((unsigned long long int)ALUin1) >> 7);
            if (temp == 1) ALUout |= (long long int)1;
            else ALUout &= ~((long long int)1);
            ALUin1 = ALUout;
        }
        OF = CF ^ ((ALUout >> 7) & 1);
    }
    if (OpMaskDest == Mask16)
    {
        int temp = 0;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            CF = ALUin1 >> 15 & 1;
            ALUout = (ALUin1 << 1) | (((unsigned long long int)ALUin1) >> 15);
            if (temp == 1) ALUout |= (long long int)1;
            else ALUout &= ~((long long int)1);
            ALUin1 = ALUout;
        }
        OF = CF ^ ((ALUout >> 15) & 1);
    }
    if (OpMaskDest == Mask32)
    {
        int temp = 0;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            CF = ALUin1 >> 31 & 1;
            ALUout = (ALUin1 << 1) | (((unsigned long long int)ALUin1) >> 31);
            if (temp == 1) ALUout |= (long long int)1;
            else ALUout &= ~((long long int)1);
            ALUin1 = ALUout;
        }
        OF = CF ^ ((ALUout >> 31) & 1);
    }
    if (OpMaskDest == Mask64)
    {
        int temp = 0;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            CF = ALUin1 >> 63 & 1;
            ALUout = (ALUin1 << 1) | (((unsigned long long int)ALUin1) >> 63);
            if (temp == 1) ALUout |= (long long int)1;
            else ALUout &= ~((long long int)1);
            ALUin1 = ALUout;
        }
        OF = CF ^ ((ALUout >> 63) & 1);
    }
}
void rcr(){
    if (OpMaskDest == Mask8l)
    {
        unsigned int temp;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            CF = ALUin1 & 1;
            ALUout = (ALUin1 >> 1) | (ALUin1 << 7);
            if (temp == 1) ALUout |= (long long int)1 << 7;
            else ALUout &= ~((long long int)1 << 7);
            ALUin1 = ALUout;        
        }
        OF = ((ALUout >> 7) & 1) ^ ((ALUout >> 6) & 1);
    }
    if (OpMaskDest == Mask16)
    {
        unsigned int temp;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            CF = ALUin1 & 1;
            ALUout = (ALUin1 >> 1) | (ALUin1 << 15);
            if (temp == 1) ALUout |= (long long int)1 << 15;
            else ALUout &= ~((long long int)1 << 15);
            ALUin1 = ALUout;
        }
        OF = ((ALUout >> 15) & 1) ^ ((ALUout >> 14) & 1);
    }
    if (OpMaskDest == Mask32)
    {
        unsigned int temp;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            CF = ALUin1 & 1;
            ALUout = (ALUin1 >> 1) | (ALUin1 << 31);
            if (temp == 1) ALUout |= (long long int)1 << 31;
            else ALUout &= ~((long long int)1 << 31);
            ALUin1 = ALUout;        
        }
        OF = ((ALUout >> 31) & 1) ^ ((ALUout >> 30) & 1);
    }
    if (OpMaskDest == Mask32)
    {
        unsigned int temp;
        for (int i = 0; i < ALUin2; i++)
        {
            temp = CF;
            CF = ALUin1 & 1;
            ALUout = (ALUin1 >> 1) | (ALUin1 << 63);
            if (temp == 1) ALUout |= (long long int)1 << 63;
            else ALUout &= ~((long long int)1 << 63);
            ALUin1 = ALUout;        
        }
        OF = ((ALUout >> 63) & 1) ^ ((ALUout >> 62) & 1);
    }
}
void sal(){
    if (OpMaskDest == Mask8l)
    {
        int bitState = (ALUin1 >> (7 - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;    
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1 && ((ALUout >> 7) & 1) == CF) { OF = 0; }
        else { OF = 1; }
    }
    if (OpMaskDest == Mask16)
    {
        int bitState = (ALUin1 >> (15 - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;    
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1 && ((ALUout >> 15) & 1) == CF) { OF = 0; }
        else { OF = 1; }
    }
    if (OpMaskDest == Mask32)
    {
        int bitState = (ALUin1 >> (31 - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;    
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1 && ((ALUout >> 31) & 1) == CF) { OF = 0; }
        else { OF = 1; }
    }
    if (OpMaskDest == Mask64)
    {
        int bitState = (ALUin1 >> (63 - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;    
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1 && ((ALUout >> 63) & 1) == CF) { OF = 0; }
        else { OF = 1; }
    }
}
void sar(){
    /* Don't have to check mask */
    if (OpMaskDest == Mask8l)
    {
        int bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = 0; }
    }
    if (OpMaskDest == Mask16)
    {        
        int bitState;
        bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = 0; }
    }
    if (OpMaskDest == Mask32)
    {        
        int bitState;
        bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = 0; }
    }
    if (OpMaskDest == Mask64)
    {
        int bitState;
        bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = 0; }
    }
}
void shl(){
    if (OpMaskDest == Mask8l)
    {
        int bitState = (ALUin1 >> (7 - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1 && ((ALUout >> 7) & 1) == CF) { OF = 0; }
        else { OF = 1; }
    }
    if (OpMaskDest == Mask16)
    {
        int bitState = (ALUin1 >> (15 - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1 && ((ALUout >> 15) & 1) == CF) { OF = 0; }
        else { OF = 1; }
    }
    if (OpMaskDest == Mask32)
    {
        int bitState = (ALUin1 >> (31 - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1 && ((ALUout >> 31) & 1) == CF) { OF = 0; }
        else { OF = 1; }
    }
    if (OpMaskDest == Mask64)
    {
        int bitState = (ALUin1 >> (63 - (ALUin2 - 1))) & 1;
        ALUout = ALUin1 << ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1 && ((ALUout >> 63) & 1) == CF) { OF = 0; }
        else { OF = 1; }
    }
}
void shr(){
    if (OpMaskDest == Mask8l)
    {
        int bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = (ALUin1 >> 7) & 1; }
    }
    if (OpMaskDest == Mask16)
    {
        int bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = (ALUin1 >> 15) & 1; }
    }
    if (OpMaskDest == Mask32)
    {
        int bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = (ALUin1 >> 31) & 1; }
    }
    if (OpMaskDest == Mask64)
    {
        int bitState = (ALUin1 >> (ALUin2 - 1)) & 1;
        ALUout = ALUin1 >> ALUin2;
        if (ALUin2 == 0) {}
        else { CF = bitState; }
        if (ALUin2 == 1) { OF = (ALUin1 >> 63) & 1; }
    }
}

/* --- CARRY INSTRUCTIONS --- */

void adc(){
    long long int temp1 = ALUout;
    long long int temp2 = ALUin1;
    long long int result = ALUout + ALUin1 + CF;
    if ((result >> 63) & 1) { SF = 1; }
    else { SF = 0; }
    if (result == 0) { ZF = 1; }
    else { ZF = 0; }
    temp2 += CF;  // Adding the carry for further EFLAGS inspection
    if (1 & ((temp1 & temp2 & ~result) | (~temp1 & ~temp2 & result) >> 63)) { CF = 1; }
    else { CF = 0; }
    if ((temp1 < 0 && temp2 > INT_MAX - temp1) ||
        (temp1 > 0 && temp2 < INT_MAX - temp1)) { OF = 1; }
    else { OF = 0; }
    ALUout = result;
}
