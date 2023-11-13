#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "x86_64.h"
#include <math.h>
#include <limits.h>

long long int RAX;                // 64-bit general purpose A register
long long int RBX;                // 64-bit general purpose B register
long long int RCX;                // 64-bit general purpose C register
long long int RDX;                // 64-bit general purpose D register
long long int RSI;                // 64-bit source index register
long long int RDI;                // 64-bit destination index register
long long int RSP;                // 64-bit stack pointer register
long long int RBP;                // 64-bit base pointer register
long long int R8;                 // 64-bit general purpose 8 register
long long int R9;                 // 64-bit general purpose 9 register
long long int R10;                // 64-bit general purpose 10 register
long long int R11;                // 64-bit general purpose 11 register
long long int R12;                // 64-bit general purpose 12 register
long long int R13;                // 64-bit general purpose 13 register
long long int R14;                // 64-bit general purpose 14 register
long long int R15;                // 64-bit general purpose 15 register
long long int RIP;                // 64-bit instruction pointer register
int OF;                           // overflow flag bit of RFLAGS
int CF;                           // carry flag bit of RFLAGS
int SF;                           // sign flag bit of RFLAGS
int ZF;                           // zero flag bit of RFLAGS

long long int ALUin1;             // inner register for ALU first input
long long int ALUin2;             // inner register for ALU second input
long long int ALUout;             // inner register for ALU result
long long int OpMaskDest;         // mask according to the destination operand
long long int OpMaskSource;       // mask according to the source operand

MemDataUnit *MemData;             // head pointer of data memory linked list
MemInstUnit *MemInst;             // head pointer of instruction memory linked list

long long int Address;            // current memory address
int RW_Size;                      // number of bytes in memory access
char Mnemonic[32];                // mnemonic of current instruction
char Operand1[32];                // simplified destination operand
char Operand2[32];                // simplified source operand
char Operand3[32];                // simplified (optional) source operand
char Assembly[256];               // text of assembly instruction
int InstLength;                   // number of bytes in the current machine code
unsigned char MachineCode[32];    // bytes of the current machine code

long long int RSPinit;            // initial value of RSP
int Run;                          // main loop flag (1: repeat fetch-execute loop; 0: terminate run)

long long int *WriteBackDest;
long long int WriteBackAddress;
int WriteBackSize;

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
                if (current->addr != address + i)
                {
                    Run = 0;
                    fprintf(stderr, "Memory read error! Not following address!");
                    exit(42);
                }                

                toreturn |= (long long int)(((long long int)(current->byte)<<(i*8)));

                if (current->next == NULL)
                    break;

                current = current->next;
            }
            return toreturn;
        }
        else
        {
            current = current->next;
        }        
    }
    fprintf(stderr, "Memory read error! Data not in memory! (%llx)\n", address);    
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
    char* file = "/tmp/asemu_output";
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

    fprintf(fp, "%llX", (RSPinit - RSP));
    fprintf(fp, "(\t");

    for (int i = 0 ; i < RSPinit - RSP ; i++)
    {
        //printf("¶¶¶¶¶¶¶ %llx ¶¶¶¶¶¶¶\n", RSPinit-i);
        fprintf(fp, "%X", ReadMem(RSPinit-i, 1));
    }
    
    fprintf(fp, ")\t");
    fprintf(fp, "%s\n", Assembly);

    fclose(fp);
}


void InstructionFetch()
{
    // Read an instruction from the instruction memory list addressed by RIP
    // Update: Mnemonic, Operand1, Operand2, Assembly, InstLength, MachineCode, RIP

    MemInstUnit *tmp = MemInst;
    while (tmp)
    {
        if (tmp->addr == RIP)
        {
            strcpy(Mnemonic, tmp->mnemonic);
            strcpy(Operand1, tmp->operand1);
            strcpy(Operand2, tmp->operand2);
            strcpy(Operand3, tmp->operand3);
            strcpy(Assembly, tmp->assembly);
            strcpy(MachineCode, tmp->machinecode);
            InstLength = tmp->instlength;
            RIP += tmp->instlength;
            return;
        }
        tmp = tmp->next;
    }
    
}
void Init()
{
    // Quick fix to reset the output file
    remove("/tmp/asemu_output");

    RAX = 0x4198694;
    RBX = 0x4198736;
    RCX = 0x4198736;
    RDX = 0x140729291588392; // rsp+256
    RDI = 0x1;
    RSI = 0x140729291588376; // rsp+240
    RSP = 0x140729291588136; // Az utolsó 3 bit biztos, hogy 0.
    RBP = 0x0;
    R8 = 0x0;
    R9 = 0x140013369068896;
    R10 = 0x0;
    R11 = 0x140662942914800;
    R12 = 0x4198464;
    R13 = 0x140729747886176;
    R14 = 0x0;
    R15 = 0x0;
    CF = 0x0;
    OF = 0x0;
    ZF = 0x1;
    SF = 0x0;

    RSPinit = RSP;
    Run = 1;

    MemData = NULL;
    MemDataUnit **LastData = malloc(sizeof(MemDataUnit));

    MemInst = NULL;
    MemInstUnit **LastInst = malloc(sizeof(MemInstUnit));

    int CodeSegment = 0; // code segment indicator in RAM.txt
    char *line = malloc(Mask8l);
    FILE* RAM = fopen("/tmp/asemu_ram", "rt");
    fgets(line, Mask8l, RAM);
    RIP = strtol(strchr(line, 32), NULL, 16);
    while (!feof(RAM))
    {
        fgets(line, Mask8l, RAM);
        if (strstr(line, ".text"))
        {
            CodeSegment = 1;
        }
        else if (strstr(line, ".rodata") || strstr(line, ".data") || strstr(line, ".bss"))
        {
            CodeSegment = 0;
        }
        else
        {
            if (CodeSegment)
            {
                MemInstUnit *tmp = malloc(sizeof(MemInstUnit));
                sscanf(line, "%llx   %s  %s  %s  %s  %s   %[^\n]", &(tmp->addr), tmp->mnemonic, tmp->operand1, tmp->operand2, tmp->operand3, tmp->machinecode, tmp->assembly);
                tmp->instlength = strlen(tmp->machinecode) / 2;
                tmp->next = NULL;

                if (!MemInst)
                {
                    MemInst = tmp;
                    *LastInst = tmp;
                }

                (*LastInst)->next = tmp;
                (*LastInst) = tmp;
            }
            else
            {
                MemDataUnit *tmp = malloc(sizeof(MemDataUnit));

                sscanf(line, "%llx  %hhx", &(tmp->addr), &(tmp->byte));
                tmp->next = NULL;

                if (!MemData)
                {
                    MemData = tmp;
                    *LastData = tmp;
                }

                (*LastData)->next = tmp;
                (*LastData) = tmp;
            }
        }
    }
}
void OperandFetch()
{
    // Load the proper values into the ALUin1 and ALUin2 based on Operand1, Operand2
    // Set the ALU masks
    // Apply constant, register content or data memory content (using proper addressing mode)
    if (Operand1[0] == '_')
        return;

    if (!strcmp(Mnemonic, "lea"))
    {
        ResolveAddress(&ALUin2, Operand1);
        return;
    }

    if (!FetchRegister(Operand1, &ALUin1, &OpMaskDest))
    {
        long long int Address;
        if (!strcmp(Mnemonic, "mov"))
        {
            if (Operand1[0] == 'Q')
            {
                WriteBackSize = 8;
                OpMaskDest = Mask64;
            }
            if (Operand1[0] == 'D')
            {
                WriteBackSize = 4;
                OpMaskDest = Mask32;
            }
            if (Operand1[0] == 'W')
            {
                WriteBackSize = 2;
                OpMaskDest = Mask16;
            }
            if (Operand1[0] == 'B')
            {
                WriteBackSize = 1;
                OpMaskDest = Mask8h;
            }
        }
        if (Operand1[0] == 'Q')
        {
            ResolveAddress(&Address, Operand1);
            ALUin1 = ReadMem(Address, 8);
            WriteBackSize = 8;
            OpMaskDest = Mask64;
        }
        else if (Operand1[0] == 'D')
        {
            ResolveAddress(&Address, Operand1);
            ALUin1 = ReadMem(Address, 4);
            WriteBackSize = 4;
            OpMaskDest = Mask32;
        }
        else if (Operand1[0] == 'W')
        {
            ResolveAddress(&Address, Operand1);
            ALUin1 = ReadMem(Address, 2);
            WriteBackSize = 2;
            OpMaskDest = Mask16;
        }
        else if (Operand1[0] == 'B')
        {
            ResolveAddress(&Address, Operand1);
            ALUin1 = ReadMem(Address, 1);
            WriteBackSize = 1;
            OpMaskDest = Mask8l;
        }
        else
        {
            FetchLiteral(Operand1, &ALUin1);
            OpMaskDest = Mask64;
        }
        WriteBackAddress = Address;
        ALUin1 = ALUin1 & OpMaskDest;
    }

    if (!FetchRegister(Operand2, &ALUin2, &OpMaskSource))
    {
        long long int Address;
        if (Operand2[0] == 'Q')
        {
            ResolveAddress(&Address, Operand2);
            ALUin2 = ReadMem(Address, 8);
            OpMaskSource = Mask64;
        }
        else if (Operand2[0] == 'D')
        {
            ResolveAddress(&Address, Operand2);
            ALUin2 = ReadMem(Address, 4);
            OpMaskSource = Mask32;
        }
        else if (Operand2[0] == 'W')
        {
            
            ResolveAddress(&Address, Operand2);
            ALUin2 = ReadMem(Address, 2);
            OpMaskSource = Mask16;
        }
        else if (Operand2[0] == 'B')
        {
            ResolveAddress(&Address, Operand2);
            ALUin2 = ReadMem(Address, 1);
            OpMaskSource = Mask8l;
        }
        else
        {
            FetchLiteral(Operand2, &ALUin2);
            OpMaskSource = Mask64;
        }
        ALUin2 = ALUin2 & OpMaskSource;
    }
}
void WriteBack()
{
    if (!strcmp(Mnemonic, "cmp"))
        return;
    if (!strcmp(Mnemonic, "test"))
        return;
    if (!strcmp(Mnemonic, "xchg"))
        return;
    if (!strcmp(Mnemonic, "imul"))
        return;
    if (!strcmp(Mnemonic, "mul"))
        return;

    ALUout = ALUout & OpMaskDest;
    long long int tmp, mask;
    if (FetchRegister(Operand1, &tmp, &mask))
    {
        *WriteBackDest = (*WriteBackDest & (Mask64 & (~mask))) | ALUout;
    }
    else
    {
        WriteMem(WriteBackAddress, WriteBackSize, ALUout);
    }
}

void add()
{
    CF = 0;
    OF = 0;
    switch (OpMaskSource)
    {
    case Mask8l:
    {
        char a, b;
        a = ALUin1;
        b = ALUin2;
        ALUout = (a + b) & OpMaskSource;
        if ((unsigned char)a > Mask8l - (unsigned char)b)
            CF = 1;
        if (a < 0 == b < 0 && (char)(a + b) < 0 != a < 0)
            OF = 1;
        break;
    }
    case Mask16:
    {
        short int a, b;
        a = ALUin1;
        b = ALUin2;
        ALUout = (a + b) & OpMaskSource;
        if ((unsigned short int)a > Mask16 - (unsigned short int)b)
            CF = 1;
        if (a < 0 == b < 0 && (short int)(a + b) < 0 != a < 0)
            OF = 1;
        break;
    }
    case Mask32:
    {
        int a, b;
        a = ALUin1;
        b = ALUin2;
        ALUout = (a + b) & OpMaskSource;
        if ((unsigned int)a > Mask32 - (unsigned int)b)
            CF = 1;
        if (a < 0 == b < 0 && (int)(a + b) < 0 != a < 0)
            OF = 1;
        break;
    }
    case Mask64:
    {
        long long int a, b;
        a = ALUin1;
        b = ALUin2;
        ALUout = (a + b) & OpMaskSource;
        if ((unsigned long long int)a > Mask64 - (unsigned long long int)b)
            CF = 1;
        if (a < 0 == b < 0 && (long long int)(a + b) < 0 != a < 0)
            OF = 1;
    }
    }
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = CheckZF(ALUout, OpMaskDest);
}
void addsd()
{
    return;
}
void and ()
{
    ALUout = ALUin1 & ALUin2;
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = CheckZF(ALUout, OpMaskDest);
    return;
}
void call()
{
    if (RSP > RSPinit) 
    {
        fprintf(stderr, "Error! RSP >= RSPinit(0x%llx < 0x%llx)\n", RSP, RSPinit); // ret javítása, oda kell tenni ezt az ellenőrzést, ugyanugy stacpointerbol adod hibak
        Run = 0;
    }
    WriteMem(RSP-8, 8, RIP); // a stack pointer oda mutat ahol már van elem, nem az első üres helyre, ret, pop, push, call javítás!!
    RSP -= 8;
    RIP = ALUin1;
    return;
}
void cbw()
{
    if (RAX & 0x80)
        RAX = RAX | 0xFF00;
    else
        RAX = RAX & 0x00FF;
    return;
}
void cdqe()
{
    if (RAX & 0x80000000)
        RAX = RAX | 0xFFFFFFFF00000000;
    else
        RAX = RAX & 0x00000000FFFFFFFF;
    return;
}
void cdq()
{
    if (RAX & 0x80000000)
        RDX = 0xFFFFFFFF;
    else
        RDX = 0;
    return;
}
void cmp()
{
    sub();
    return;
}
void cwd()
{
    if (RAX & 0x8000)
        RDX = 0xFFFF;
    else
        RDX = 0;
    return;
}
void dec()
{
    ALUin2 = 1;
    sub();
    return;
}
// Quotient of signed division (128-bit / 64-bit = 64-bit)
long long int quotient64_signed(long long int dividend, long long int divisor)
{
    asm("movq %rsi, %rbx \n"
        "movq %rdi, %rax \n"
        "cqto \n"
        "idivq %rbx \n");
    return;
}

// Reminder of signed division (128-bit % 64-bit = 64-bit)
long long int reminder64_signed(long long int dividend, long long int divisor)
{
    asm("movq %rsi, %rbx \n"
        "movq %rdi, %rax \n"
        "cqto \n"
        "idivq %rbx \n"
        "movq %rdx, %rax");
    return;
}

// Signed division in x86-64 ALU
void assembly_idiv()
{
    if (OpMaskSource == Mask64)
    { // 64-bit case
        RAX = quotient64_signed(ALUin1, ALUin2);
        RDX = reminder64_signed(ALUin1, ALUin2);
    }
    
        else if (OpMaskSource == Mask32)
    {
        RAX = (ALUin1/ALUin2) & Mask32;
        RDX = (ALUin1%ALUin2) & Mask32;
    }
    else if (OpMaskSource == Mask16)
    {
        RAX = (ALUin1/ALUin2) & Mask16;
        RDX = (ALUin1%ALUin2) & Mask16;
    }
    else if (OpMaskSource == Mask8l)
    {
        RAX = (ALUin1/ALUin2) & Mask8l;
        RDX = (ALUin1%ALUin2) & Mask8l;
    }
    
}

// Quotient of unsigned division (128-bit / 64-bit = 64-bit)
long long int quotient64_unsigned(long long int dividend, long long int divisor)
{
    asm("movq %rsi, %rbx \n"
        "movq %rdi, %rax \n"
        "cqto \n"
        "divq %rbx \n");
    return;
}

// Reminder of unsigned division (128-bit % 64-bit = 64-bit)
long long int reminder64_unsigned(long long int dividend, long long int divisor)
{
    asm("movq %rsi, %rbx \n"
        "movq %rdi, %rax \n"
        "cqto \n"
        "divq %rbx \n"
        "movq %rdx, %rax");
    return;
}

// Unsigned division in x86-64 ALU
void assembly_div()
{
    if (OpMaskSource == Mask64)
    { // 64-bit case
        if (ALUin1 & 8000000000000000 || ALUin2 & 8000000000000000)
        {
            fprintf(stderr, " Negative operand of unsigned division.\n");
            return;
        }
        RAX = quotient64_unsigned(ALUin1, ALUin2);
        RDX = reminder64_unsigned(ALUin1, ALUin2);
    }
    else if (OpMaskSource == Mask32)
    {
        if (ALUin1 & 80000000 || ALUin2 & 80000000)
        {
            fprintf(stderr, " Negative operand of unsigned division.\n");
            return;
        }
        RAX = (ALUin1/ALUin2) & Mask32;
        RDX = (ALUin1%ALUin2) & Mask32;
    }
    else if (OpMaskSource == Mask16)
    {
        if (ALUin1 & 8000 || ALUin2 & 8000)
        {
            fprintf(stderr, " Negative operand of unsigned division.\n");
            return;
        }
        RAX = (ALUin1/ALUin2) & Mask16;
        RDX = (ALUin1%ALUin2) & Mask16;
    }
    else if (OpMaskSource == Mask8l)
    {
        if (ALUin1 & 80 || ALUin2 & 80)
        {
            fprintf(stderr, " Negative operand of unsigned division.\n");
            return;
        }
        RAX = (ALUin1/ALUin2) & Mask8l;
        RDX = (ALUin1%ALUin2) & Mask8l;
    }
}

void inc()
{
    ALUin2 = 1;
    add();
    return;
}
void lea()
{
    ALUout = ALUin2; // preliminary calculations performed by Operandfetch()
    return;
}
void leave()
{
    RSP = RBP;
    RBP = ReadMem(RSP + 8, 8);
    return;
}
void loop()
{
    if (RCX > 0)
    {
        jmp();
        RCX--;
    }
    return;
}
void movabs()
{
    ALUout = ALUin2;
    return;
}
void mov()
{
    long long int tmp, mask;
    if (FetchRegister(Operand1, &tmp, &mask))
    {
        ALUout = ALUin2;
    }
    else
    {
        WriteMem(ALUin1+1, WriteBackSize, ALUin2);
    }
    return;
}
void movsx()
{
    if (CheckSF(ALUin2, OpMaskSource))
    {
        ALUout = (OpMaskSource & OpMaskDest) | ALUin2;
    }
    else
    {
        ALUout = (~OpMaskDest) | ALUin2;
    }
    return;
}
void movzx()
{
    ALUout = (~OpMaskDest) | ALUin2;
    return;
}
long long int mul64_rdx(long long int multiplicand, long long int multiplier)
{
    asm("movq %rsi, %rbx \n"
        "movq %rdi, %rax \n"
        "mulq %rbx \n"
        "movq %rdx, %rax");
    return;
}
long long int mul64_rax(long long int multiplicand, long long int multiplier)
{
    asm("movq %rsi, %rbx \n"
        "movq %rdi, %rax \n"
        "mulq %rbx \n");
    return;
}
long long int imul64_rdx(long long int multiplicand, long long int multiplier)
{
    asm("movq %rsi, %rbx \n"
        "movq %rdi, %rax \n"
        "imulq %rbx \n"
        "movq %rdx, %rax");
    return;
}
long long int imul64_rax(long long int multiplicand, long long int multiplier)
{
    asm("movq %rsi, %rbx \n"
        "movq %rdi, %rax \n"
        "imulq %rbx \n");
    return;
}
void mul()
{
    if (OpMaskDest == Mask64)
    {
        RAX = mul64_rax(ALUin1, ALUin2);
        RDX = mul64_rdx(ALUin1, ALUin2);
        CF = OF = RDX ? 1 : 0;
    }
    else if (OpMaskDest == Mask32)
    {
        RAX = (long long unsigned int)(RAX & 0xffffffff00000000) | (ALUin1 * ALUin2) & Mask32;
        RDX = (long long unsigned int)(RDX & 0xffffffff00000000) | ((ALUin1 * ALUin2) & (~Mask32)) >> 32;
        CF = OF = (RDX & Mask32) ? 1 : 0;
    }
    else if (OpMaskDest == Mask16)
    {
        RAX = (long long unsigned int)(RAX & 0xffffffffffff0000) | (ALUin1 * ALUin2) & Mask16;
        RDX = (long long unsigned int)(RDX & 0xffffffffffff0000) | ((ALUin1 * ALUin2) & (~Mask16)) >> 16;
        CF = OF = (RDX & Mask16) ? 1 : 0;
    }
    else if (OpMaskDest == Mask8l)
    {
        RAX = (long long unsigned int)(RAX & 0xffffffffffff0000) | (ALUin1 * ALUin2) & Mask16;
        CF = OF = (RAX & Mask8h) ? 1 : 0;
    }
    return;
}
void imul()
{
    if (OpMaskDest == Mask64)
    {
        RAX = imul64_rax(ALUin1, ALUin2);
        RDX = imul64_rdx(ALUin1, ALUin2);
        CF = OF = RDX ? 1 : 0;
    }
    else if (OpMaskDest == Mask32)
    {
        RAX = (RAX & 0xffffffff00000000) | (ALUin1 * ALUin2) & Mask32;
        RDX = (RDX & 0xffffffff00000000) | ((ALUin1 * ALUin2) & (~Mask32)) >> 32;
        CF = OF = (RDX & Mask32) ? 1 : 0;
    }
    else if (OpMaskDest == Mask16)
    {
        RAX = (RAX & 0xffffffffffff0000) | (ALUin1 * ALUin2) & Mask16;
        RDX = (RDX & 0xffffffffffff0000) | ((ALUin1 * ALUin2) & (~Mask16)) >> 16;
        CF = OF = (RDX & Mask16) ? 1 : 0;
    }
    else if (OpMaskDest == Mask8l)
    {
        RAX = (RAX & 0xffffffffffff0000) | (ALUin1 * ALUin2) & Mask16;
        CF = OF = (RAX & Mask8h) ? 1 : 0;
    }
    return;
}
void neg()
{
    ALUin2 = ~ALUin1; 
    ALUin1 = 1;
    add();
    return;
}
void nop() // ha van operandus, az csak annak a száma, hogy hány nop fog következni egymás után
{
    return;
}
void not()
{
    ALUout = ~ALUin1;
    return;
}
void or ()
{
    ALUout = ALUin1 | ALUin2;
    return;
}
void test()
{
    and();
}
void xchg()
{
    long long int tmp, mask;
    if (FetchRegister(Operand1, &tmp, &mask))
    {
        *WriteBackDest = ALUin2;
    }
    else
    {
        WriteMem(WriteBackAddress, WriteBackSize, ALUin2);
    }
    if (FetchRegister(Operand2, &tmp, &mask))
    {
        *WriteBackDest = ALUin1;
    }
    else
    {
        WriteMem(WriteBackAddress, WriteBackSize, ALUin1);
    }
    return;
}
void xor () {
    ALUout = ALUin1 ^ ALUin2;
    CF = OF = 0;
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = CheckZF(ALUout, OpMaskDest);
} 
void enbdr64()
{
    return;
}

// ###############################//
void sub()
{
    long long int Sbit = 0x80;
    CF = 0;
    OF = 0;
    switch (OpMaskSource)
    {
    case 0x000000000000ff:
    {
        char a, b;
        a = ALUin1;
        b = ALUin2;
        ALUout = (a - b) & OpMaskSource;
        if ((unsigned char)a < (unsigned char)b)
            CF = 1;
        if (a < 0 != b < 0 && (char)(a - b) < 0 != a < 0)
            OF = 1;
        break;
    }
    case 0x0000000000ffff:
    {
        short int a, b;
        a = ALUin1;
        b = ALUin2;
        ALUout = (a - b) & OpMaskSource;
        if ((unsigned short int)a < (unsigned short int)b)
            CF = 1;
        if (a < 0 != b < 0 && (short int)(a - b) < 0 != a < 0)
            OF = 1;
        break;
    }
    case 0x00000000ffffffff:
    {
        int a, b;
        a = ALUin1;
        b = ALUin2;
        ALUout = (a - b) & OpMaskSource;
        if ((unsigned int)a < (unsigned int)b)
            CF = 1;
        if (a < 0 != b < 0 && (int)(a - b) < 0 != a < 0)
            OF = 1;
        break;
    }
    case 0xffffffffffffffff:
    {
        long long int a, b;
        a = ALUin1;
        b = ALUin2;
        ALUout = (a - b) & OpMaskSource;
        if ((unsigned long long int)a < (unsigned long long int)b)
            CF = 1;
        if (a < 0 != b < 0 && (long long int)(a - b) < 0 != a < 0)
            OF = 1;
    }
    }
    while (Sbit * 2 - 1 != OpMaskSource)
        Sbit <<= 1;
    if ((ALUout & Sbit) != 0)
        SF = 1;
    else
        SF = 0;
    if (ALUout == 0)
        ZF = 1;
    else
        ZF = 0;
}
void sbb()
{
    ALUin2 += CF;
    sub();
}
void ret()
{
    if (RSP > RSPinit) 
    {
        fprintf(stderr, "Error! RSP > RSPinit(0x%llx < 0x%llx)\n", RSP, RSPinit); // ret javítása, oda kell tenni ezt az ellenőrzést, ugyanugy stacpointerbol adod hibak
        Run = 0;
    }
    if (RSP == RSPinit)
    {
        Run = 0;
        return;
    }
    RIP = ReadMem(RSP, 8);
    RSP += 8;
    return;
}

void push()
{
    int size;
    switch (OpMaskDest)
    {
    case Mask64:
        size = 8;
        break;
    case Mask32:
        size = 4;
        break;
    case Mask16:
        size = 2;
        break;
    case Mask8h || Mask8l:
        size = 1;
        break;
    default:
        break;
    }
    RSP -= size;
    WriteMem(RSP+1, size, ALUin1);
    //printf("%llX címre pusholt elem %d bájt hosszan: %llX\n", RSP, size, ALUin1);

    
    // printf("RSP: %llX\n", RSP);
    return;
}
void pop()
{
    int size;
    switch (OpMaskDest)
    {
    case Mask64:
        size = 8;
        break;
    case Mask32:
        size = 4;
        break;
    case Mask16:
        size = 2;
        break;
    case Mask8h || Mask8l:
        size = 1;
        break;
    default:
        break;
    }
    ALUout = ReadMem(RSP+1, size);
    // printf("%llX címről popolt elem %d bájt hosszan: %llX\n", RSP+size, size, ALUout);
    RSP += size;
    // printf("RSP: %llX\n", RSP);
    return;
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
    if (OF == 1) ALUout = 1;
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
        mask = 63;
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

void Execute()
{
    if (RSP > RSPinit)
    {
        Run = 0;
        fprintf(stderr, "Error! RSP (%llX) > RSPinit (%llX)\n", RSP, RSPinit);
    }
    if (!strcmp(Mnemonic, "add"))
    {
        add();
        return;
    }

    if (!strcmp(Mnemonic, "addsd"))
    {
        addsd();
        return;
    }
    if (!strcmp(Mnemonic, "and"))
    {
        and();
        return;
    }
    if (!strcmp(Mnemonic, "call"))
    {
        call();
        return;
    }
    if (!strcmp(Mnemonic, "cbw"))
    {
        cbw();
        return;
    }
    if (!strcmp(Mnemonic, "cdq"))
    {
        cdq();
        return;
    }
    if (!strcmp(Mnemonic, "cdqe"))
    {
        cdqe();
        return;
    }
    if (!strcmp(Mnemonic, "cmp"))
    {
        cmp();
        return;
    }
    if (!strcmp(Mnemonic, "cwd"))
    {
        cwd();
        return;
    }
    if (!strcmp(Mnemonic, "dec"))
    {
        dec();
        return;
    }
    if (!strcmp(Mnemonic, "div"))
    {
        assembly_div();
        return;
    }
    if (!strcmp(Mnemonic, "idiv"))
    {
        assembly_idiv();
        return;
    }
    if (!strcmp(Mnemonic, "inc"))
    {
        inc();
        return;
    }
    if (!strcmp(Mnemonic, "imul"))
    {
        imul();
        return;
    }
    if (!strcmp(Mnemonic, "lea"))
    {
        lea();
        return;
    }
    if (!strcmp(Mnemonic, "leave"))
    {
        leave();
        return;
    }
    if (!strcmp(Mnemonic, "loop"))
    {
        loop();
        return;
    }
    if (!strcmp(Mnemonic, "mov"))
    {
        mov();
        return;
    }
    if (!strcmp(Mnemonic, "movabs"))
    {
        movabs();
        return;
    }
    if (!strcmp(Mnemonic, "movsx"))
    {
        movsx();
        return;
    }
    if (!strcmp(Mnemonic, "movzx"))
    {
        movzx();
        return;
    }
    if (!strcmp(Mnemonic, "mul"))
    {
        mul();
        return;
    }
    if (!strcmp(Mnemonic, "neg"))
    {
        neg();
        return;
    }
    if (!strcmp(Mnemonic, "not"))
    {
        not();
        return;
    }
    if (!strcmp(Mnemonic, "nop"))
    {
        nop();
        return;
    }
    if (!strcmp(Mnemonic, "sub"))
    {
        sub();
        if (!strcmp(Operand1, "rsp"))
        {
            for(int i = 0; i<=ALUin2; i++)
            {
                WriteMem(RSP-i, 1, 0);
            }
        }
        return;
    }
    if (!strcmp(Mnemonic, "sbb"))
    {
        sbb();
        return;
    }
    if (!strcmp(Mnemonic, "or"))
    {
        or();
        return;
    }
    if (!strcmp(Mnemonic, "test"))
    {
        test();
        return;
    }
    if (!strcmp(Mnemonic, "xchg"))
    {
        xchg();
        return;
    }
    if (!strcmp(Mnemonic, "xor"))
    {
        xor();
        return;
    }
    if (!strcmp(Mnemonic, "endbr64"))
    {
        enbdr64();
        return;
    }

    // ##########################
    if (!strcmp(Mnemonic, "ret"))
    {
        ret();
        return;
    }
    if (!strcmp(Mnemonic, "push"))
    {
        push();
        return;
    }
    if (!strcmp(Mnemonic, "pop"))
    {
        pop();
        return;
    }

    /* --- JUMP INSTRUCTIONS ---*/

    if (!strcmp(Mnemonic, "ja"))
    {
        ja();
        return;
    }
    if (!strcmp(Mnemonic, "jae"))
    {
        jae();
        return;
    }
    if (!strcmp(Mnemonic, "jb"))
    {
        jb();
        return;
    }
    if (!strcmp(Mnemonic, "jbe"))
    {
        jbe();
        return;
    }
    if (!strcmp(Mnemonic, "jc"))
    {
        jc();
        return;
    }
    if (!strcmp(Mnemonic, "je"))
    {
        je();
        return;
    }
    if (!strcmp(Mnemonic, "jg"))
    {
        jg();
        return;
    }
    if (!strcmp(Mnemonic, "jge"))
    {
        jge();
        return;
    }
    if (!strcmp(Mnemonic, "jl"))
    {
        jl();
        return;
    }
    if (!strcmp(Mnemonic, "jle"))
    {
        jle();
        return;
    }
    if (!strcmp(Mnemonic, "jmp"))
    {
        jmp();
        return;
    }
    if (!strcmp(Mnemonic, "jnc"))
    {
        jnc();
        return;
    }
    if (!strcmp(Mnemonic, "jne"))
    {
        jne();
        return;
    }
    if (!strcmp(Mnemonic, "jnz"))
    {
        jnz();
        return;
    }
    if (!strcmp(Mnemonic, "jz"))
    {
        jz();
        return;
    }

    /* --- SET INSTRUCTIONS --- */

    if (!strcmp(Mnemonic, "seta"))
    {
        seta();
        return;
    }
    if (!strcmp(Mnemonic, "setae"))
    {
        setae();
        return;
    }
    if (!strcmp(Mnemonic, "setb"))
    {
        setb();
        return;
    }
    if (!strcmp(Mnemonic, "setbe"))
    {
        setbe();
        return;
    }
    if (!strcmp(Mnemonic, "setc"))
    {
        setc();
        return;
    }
    if (!strcmp(Mnemonic, "sete"))
    {
        sete();
        return;
    }
    if (!strcmp(Mnemonic, "setg"))
    {
        setg();
        return;
    }
    if (!strcmp(Mnemonic, "setge"))
    {
        setge();
        return;
    }
    if (!strcmp(Mnemonic, "setl"))
    {
        setl();
        return;
    }
    if (!strcmp(Mnemonic, "setle"))
    {
        setle();
        return;
    }
    if (!strcmp(Mnemonic, "setnc"))
    {
        setnc();
        return;
    }
    if (!strcmp(Mnemonic, "setne"))
    {
        setne();
        return;
    }
    if (!strcmp(Mnemonic, "setnz"))
    {
        setnz();
        return;
    }
    if (!strcmp(Mnemonic, "seto"))
    {
        seto();
        return;
    }
    if (!strcmp(Mnemonic, "setz"))
    {
        setz();
        return;
    }
    
    /* --- SHIFT AND ROTATE INSTRUCTIONS --- */

    if (!strcmp(Mnemonic, "rol"))
    {
        rol();
        return;
    }
    if (!strcmp(Mnemonic, "ror"))
    {
        ror();
        return;
    }
    if (!strcmp(Mnemonic, "rcl"))
    {
        rcl();
        return;
    }
    if (!strcmp(Mnemonic, "rcr"))
    {
        rcr();
        return;
    }
    if (!strcmp(Mnemonic, "sal"))
    {
        sal();
        return;
    }
    if (!strcmp(Mnemonic, "sar"))
    {
        sar();
        return;
    }
    if (!strcmp(Mnemonic, "shl"))
    {
        shl();
        return;
    }
    if (!strcmp(Mnemonic, "shr"))
    {
        shr();
        return;
    }
    
    /* --- CARRY INSTRUCTIONS --- */
    
    if (!strcmp(Mnemonic, "adc"))
    {
        adc();
        return;
    }

    printf("Error! Instruction not implemented (%s)\n", Mnemonic);
    //Run = 0;
}
void Fini()
{
    SaveState();
    puts("###--Fini--###");
}

// #####################################################################//

int CheckSF(int Value, long long int Mask)
{
    if (Mask == Mask64)
        return (Value & 0x8000000000000000) >> 63;
    if (Mask == Mask32)
        return (Value & 0x80000000) >> 31;
    if (Mask == Mask16)
        return (Value & 0x8000) >> 15;
    if (Mask == Mask8l)
        return (Value & 0x80) >> 7;
    if (Mask == Mask8h)
        return (Value & 0x8000) >> 15;
}
int CheckZF(int Value, long long int Mask)
{
    if (Mask == Mask64)
        return (Mask64 & Value) ? 0 : 1;
    if (Mask == Mask32)
        return (Mask32 & Value) ? 0 : 1;
    if (Mask == Mask16)
        return (Mask16 & Value) ? 0 : 1;
    if (Mask == Mask8l)
        return (Mask8l & Value) ? 0 : 1;
    if (Mask == Mask8h)
        return ((Mask8h & Value)) ? 0 : 1;
}

int FetchRegister(char *Operand, long long int *Destination, long long int *Mask)
{
    if (!strcmp(Operand, "rax"))
    {
        *Destination = RAX;
        WriteBackDest = &RAX;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "eax"))
    {
        *Destination = RAX;
        WriteBackDest = &RAX;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "ax"))
    {
        *Destination = RAX;
        WriteBackDest = &RAX;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "ah"))
    {
        *Destination = RAX;
        WriteBackDest = &RAX;
        *Mask = Mask8h;
        return 1;
    }
    if (!strcmp(Operand, "al"))
    {
        *Destination = RAX;
        WriteBackDest = &RAX;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rbx"))
    {
        *Destination = RBX;
        WriteBackDest = &RBX;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "ebx"))
    {
        *Destination = RBX;
        WriteBackDest = &RBX;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "bx"))
    {
        *Destination = RBX;
        WriteBackDest = &RBX;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "bh"))
    {
        *Destination = RBX;
        WriteBackDest = &RBX;
        *Mask = Mask8h;
        return 1;
    }
    if (!strcmp(Operand, "bl"))
    {
        *Destination = RBX;
        WriteBackDest = &RBX;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rcx"))
    {
        *Destination = RCX;
        WriteBackDest = &RCX;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "ecx"))
    {
        *Destination = RCX;
        WriteBackDest = &RCX;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "cx"))
    {
        *Destination = RCX;
        WriteBackDest = &RCX;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "ch"))
    {
        *Destination = RCX;
        WriteBackDest = &RCX;
        *Mask = Mask8h;
        return 1;
    }
    if (!strcmp(Operand, "cl"))
    {
        *Destination = RCX;
        WriteBackDest = &RCX;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rdx"))
    {
        *Destination = RDX;
        WriteBackDest = &RDX;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "edx"))
    {
        *Destination = RDX;
        WriteBackDest = &RDX;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "dx"))
    {
        *Destination = RDX;
        WriteBackDest = &RDX;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "dh"))
    {
        *Destination = RDX;
        WriteBackDest = &RDX;
        *Mask = Mask8h;
        return 1;
    }
    if (!strcmp(Operand, "dl"))
    {
        *Destination = RDX;
        WriteBackDest = &RDX;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rdi"))
    {
        *Destination = RDI;
        WriteBackDest = &RDI;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "edi"))
    {
        *Destination = RDI;
        WriteBackDest = &RDI;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "di"))
    {
        *Destination = RDI;
        WriteBackDest = &RDI;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "dil"))
    {
        *Destination = RDI;
        WriteBackDest = &RDI;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rsi"))
    {
        *Destination = RSI;
        WriteBackDest = &RSI;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "esi"))
    {
        *Destination = RSI;
        WriteBackDest = &RSI;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "si"))
    {
        *Destination = RSI;
        WriteBackDest = &RSI;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "sil"))
    {
        *Destination = RSI;
        WriteBackDest = &RSI;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rbp"))
    {
        *Destination = RBP;
        WriteBackDest = &RBP;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "ebp"))
    {
        *Destination = RBP;
        WriteBackDest = &RBP;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "bp"))
    {
        *Destination = RBP;
        WriteBackDest = &RBP;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "bpl"))
    {
        *Destination = RBP;
        WriteBackDest = &RBP;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rsp"))
    {
        *Destination = RSP;
        WriteBackDest = &RSP;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "esp"))
    {
        *Destination = RSP;
        WriteBackDest = &RSP;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "sp"))
    {
        *Destination = RSP;
        WriteBackDest = &RSP;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "spl"))
    {
        *Destination = RSP;
        WriteBackDest = &RSP;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r8"))
    {
        *Destination = R8;
        WriteBackDest = &R8;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r8d"))
    {
        *Destination = R8;
        WriteBackDest = &R8;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r8w"))
    {
        *Destination = R8;
        WriteBackDest = &R8;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r8b"))
    {
        *Destination = R8;
        WriteBackDest = &R8;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r9"))
    {
        *Destination = R9;
        WriteBackDest = &R9;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r9d"))
    {
        *Destination = R9;
        WriteBackDest = &R9;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r9w"))
    {
        *Destination = R9;
        WriteBackDest = &R9;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r9b"))
    {
        *Destination = R9;
        WriteBackDest = &R9;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r10"))
    {
        *Destination = R10;
        WriteBackDest = &R10;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r10d"))
    {
        *Destination = R10;
        WriteBackDest = &R10;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r10w"))
    {
        *Destination = R10;
        WriteBackDest = &R10;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r10b"))
    {
        *Destination = R10;
        WriteBackDest = &R10;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r11"))
    {
        *Destination = R11;
        WriteBackDest = &R11;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r11d"))
    {
        *Destination = R11;
        WriteBackDest = &R11;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r11w"))
    {
        *Destination = R11;
        WriteBackDest = &R11;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r11b"))
    {
        *Destination = R11;
        WriteBackDest = &R11;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r12"))
    {
        *Destination = R12;
        WriteBackDest = &R12;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r12d"))
    {
        *Destination = R12;
        WriteBackDest = &R12;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r12w"))
    {
        *Destination = R12;
        WriteBackDest = &R12;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r12b"))
    {
        *Destination = R12;
        WriteBackDest = &R12;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r13"))
    {
        *Destination = R13;
        WriteBackDest = &R13;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r13d"))
    {
        *Destination = R13;
        WriteBackDest = &R13;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r13w"))
    {
        *Destination = R13;
        WriteBackDest = &R13;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r13b"))
    {
        *Destination = R13;
        WriteBackDest = &R13;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r14"))
    {
        *Destination = R14;
        WriteBackDest = &R14;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r14d"))
    {
        *Destination = R14;
        WriteBackDest = &R14;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r14w"))
    {
        *Destination = R14;
        WriteBackDest = &R14;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r14b"))
    {
        *Destination = R14;
        WriteBackDest = &R14;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r15"))
    {
        *Destination = R15;
        WriteBackDest = &R15;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r15d"))
    {
        *Destination = R15;
        WriteBackDest = &R15;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r15w"))
    {
        *Destination = R15;
        WriteBackDest = &R15;
        *Mask = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r15b"))
    {
        *Destination = R15;
        WriteBackDest = &R15;
        *Mask = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rip"))
    {
        *Destination = RIP;
        WriteBackDest = &RIP;
        *Mask = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "eip"))
    {
        *Destination = RIP;
        WriteBackDest = &RIP;
        *Mask = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "ip"))
    {
        *Destination = RIP;
        WriteBackDest = &RIP;
        *Mask = Mask16;
        return 1;
    }
    return 0;
}
int FetchLiteral(char *Expression, long long int *Destination)
{
    if (Expression[1] == 'x')
    {
        *Destination = strtol(strrchr(Expression, 120) + 1, NULL, 16);
        return 1;
    }
    else if (strtol(Expression, NULL, 16))
    {
        *Destination = strtol(Expression, NULL, 16);
        return 1;
    }
    return 0;
}
int ResolveAddress(long long int *Destination, char* Operand)
{
    //printf("%s\n", Operand);
    char Exp[32];
    if (!sscanf(Operand, "%*[^[][%31[^]]", Exp))
        sscanf(Operand, "%*c%31[^]]", Exp); // lea miatt, mert ott a kifejezés egyből [- el kezdődik
    int Mult;
    long long int Mask;
    char Op1[32], Op2[32], Literal[32];
    long long int Reg1, Reg2, Value;
    char *Pointer1, *Pointer2, *Pointer3, *Pointer4;
    Pointer1 = strchr(Exp, '+');
    Pointer2 = strchr(Exp, '*');
    if (Pointer1)
    {
        Pointer3 = strchr(Pointer1 + 1, '+');
    }
    else
        Pointer3 = NULL;
    Pointer4 = strchr(Exp, '-');
    // printf("1:%s 2:%s 3:%s 4:%s\n", Pointer1, Pointer2, Pointer3, Pointer4);

    if (Pointer1 && Pointer2 && Pointer3) // [ <register> "+" <register> [ "*1" | "*2" | "*4" | "*8" ]  "+" <literal> ]
    {
        sscanf(Exp, "%[^+]+%[^*]*%d+%s", Op1, Op2, &Mult, Literal);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchRegister(Op2, &Reg2, &Mask);
        Reg2 &= Mask;
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 + Reg2 * Mult + Value;
        // printf("%lld, %lld, %d, %llx\n", Reg1, Reg2, Mult, Value);
        return 10;
    }
    if (Pointer1 && Pointer2 && Pointer4) // [ <register> "+" <register> [ "*1" | "*2" | "*4" | "*8" ]  "-" <literal> ]
    {
        sscanf(Exp, "%[^+]+%[^*]*%d-%s", Op1, Op2, &Mult, Literal);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchRegister(Op2, &Reg2, &Mask);
        Reg2 &= Mask;
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 + Reg2 * Mult - Value;
        printf("%lld, %lld, %d, %llx\n", Reg1, Reg2, Mult, Value);
        return 11;
    }
    if (Pointer1 && Pointer2 && Pointer1 < Pointer2) // [ <register> "+" <register> [ "*1" | "*2" | "*4" | "*8" ]]
    {
        sscanf(Exp, "%[^+]+%[^*]*%d", Op1, Op2, &Mult);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchRegister(Op2, &Reg2, &Mask);
        Reg2 &= Mask;
        *Destination = Reg1 + Reg2 * Mult;
        // printf("%lld, %lld, %d\n", Reg1, Reg2, Mult);
        return 12;
    }
    if (Pointer1 && Pointer2 && Pointer1 > Pointer2) // [<register> [ "*1" | "*2" | "*4" | "*8" ]  "+" <literal> ]
    {
        sscanf(Exp, "%[^*]*%d+%s", Op1, &Mult, Literal);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 * Mult + Value;
        //printf("%lld, %lld, %d\n", Reg1, Value, Mult);
        return 13;
    }
    if (Pointer2 && Pointer4 && Pointer4 > Pointer2) // [<register> [ "*1" | "*2" | "*4" | "*8" ]  "-" <literal> ]
    {
        sscanf(Exp, "%[^*]*%d-%s", Op1, &Mult, Literal);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 * Mult - Value;
        // printf("%lld, %lld, %d\n", Reg1, Value, Mult);
        return 14;
    }
    if (Pointer1) // [<register> "+" <literal> ]
    {
        sscanf(Exp, "%[^+]+%s", Op1, Literal);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 + Value;
        // printf("%llx, %llx\n", Reg1, Value);
        return 15;
    }
    if (Pointer4) // [<register> "-" <literal> ]
    {
        sscanf(Exp, "%[^-]-%s", Op1, Literal);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 - Value;
        // printf("%lld, %lld\n", Reg1, Value);
        return 16;
    }
    if (FetchRegister(Exp, &Reg1, &Mask))
    {
        // printf("%lld\n", Reg1);
        Reg1 &= Mask;
        *Destination = Reg1;
        return 17;
    }
    if (FetchLiteral(Exp, &Value))
    {
        // printf("%lld\n", Value);
        *Destination = Value;
        return 18;
    }
    return 19;
}
void PrintList(char *s) // "data" - print MemData, "inst" - print MemInst
{
    if (!strcmp("data", s))
    {
        MemDataUnit *tmp = MemData;
        while (tmp)
        {
            printf(" %llX \t %X \n", tmp->addr, tmp->byte);
            tmp = tmp->next;
        }
        return;
    }
    if (!strcmp("inst", s))
    {
        MemInstUnit *tmp = MemInst;
        while (tmp)
        {
            printf("%llX - %s - %s - %s - %s - %s - %s - %d\n", tmp->addr, tmp->mnemonic, tmp->operand1, tmp->operand2, tmp->operand3, tmp->machinecode, tmp->assembly, tmp->instlength);
            tmp = tmp->next;
        }
        return;
    }
    puts("PrintList: invalid parameter!");
}
