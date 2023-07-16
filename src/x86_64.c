void InstructionFetch()
{
// Read an instruction from the instruction memory list addressed by RIP
// Update: Mnemonic, Operand1, Operand2, Assembly, InstLength, MachineCode, RIP

 MemInstUnit* tmp = MemInst;
    while (tmp)
    {
        if(tmp->addr == RIP)
        {
            strcpy(Mnemonic, tmp->mnemonic);
            strcpy(Operand1, tmp->operand1);
            strcpy(Operand2, tmp->operand2);
            strcpy(Operand3, tmp->operand3);
            strcpy(Assembly, tmp->assembly);
            strcpy(MachineCode, tmp->machinecode);
            RIP = (tmp->next)->addr;
            InstLength = tmp->instlength;
            break;
        }
        tmp = tmp->next;
    }
    Run = 0;
}
int FetchRegister(char* Operand, long long int* Destination)
{
    if (!strcmp(Operand, "rax"))
    {
        *Destination = RAX;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "eax"))
    {
        *Destination = RAX;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "ax"))
    {
        *Destination = RAX;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "ah"))
    {
        *Destination = RAX;
        OpMaskDest = Mask8h;
        return 1;
    }
    if (!strcmp(Operand, "al"))
    {
        *Destination = RAX;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rbx"))
    {
        *Destination = RBX;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "ebx"))
    {
        *Destination = RBX;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "bx"))
    {
        *Destination = RBX;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "bh"))
    {
        *Destination = RBX;
        OpMaskDest = Mask8h;
        return 1;
    }
    if (!strcmp(Operand, "bl"))
    {
        *Destination = RBX;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rcx"))
    {
        *Destination = RCX;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "ecx"))
    {
        *Destination = RCX;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "cx"))
    {
        *Destination = RCX;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "ch"))
    {
        *Destination = RCX;
        OpMaskDest = Mask8h;
        return 1;
    }
    if (!strcmp(Operand, "cl"))
    {
        *Destination = RCX;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rdx"))
    {
        *Destination = RDX;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "edx"))
    {
        *Destination = RDX;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "dx"))
    {
        *Destination = RDX;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "dh"))
    {
        *Destination = RDX;
        OpMaskDest = Mask8h;
        return 1;
    }
    if (!strcmp(Operand, "dl"))
    {
        *Destination = RDX;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rdi"))
    {
        *Destination = RDI;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "edi"))
    {
        *Destination = RDI;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "di"))
    {
        *Destination = RDI;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "dil"))
    {
        *Destination = RDI;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rsi"))
    {
        *Destination = RSI;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "esi"))
    {
        *Destination = RSI;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "si"))
    {
        *Destination = RSI;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "sil"))
    {
        *Destination = RSI;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rbp"))
    {
        *Destination = RBP;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "ebp"))
    {
        *Destination = RBP;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "bp"))
    {
        *Destination = RBP;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "bpl"))
    {
        *Destination = RBP;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rsp"))
    {
        *Destination = RSP;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "esp"))
    {
        *Destination = RSP;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "sp"))
    {
        *Destination = RSP;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "spl"))
    {
        *Destination = RSP;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r8"))
    {
        *Destination = R8;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r8d"))
    {
        *Destination = R8;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r8w"))
    {
        *Destination = R8;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r8b"))
    {
        *Destination = R8;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r9"))
    {
        *Destination = R9;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r9d"))
    {
        *Destination = R9;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r9w"))
    {
        *Destination = R9;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r9b"))
    {
        *Destination = R9;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r10"))
    {
        *Destination = R10;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r10d"))
    {
        *Destination = R10;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r10w"))
    {
        *Destination = R10;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r10b"))
    {
        *Destination = R10;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r11"))
    {
        *Destination = R11;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r11d"))
    {
        *Destination = R11;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r11w"))
    {
        *Destination = R11;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r11b"))
    {
        *Destination = R11;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r12"))
    {
        *Destination = R12;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r12d"))
    {
        *Destination = R12;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r12w"))
    {
        *Destination = R12;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r12b"))
    {
        *Destination = R12;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r13"))
    {
        *Destination = R13;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r13d"))
    {
        *Destination = R13;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r13w"))
    {
        *Destination = R13;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r13b"))
    {
        *Destination = R13;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r14"))
    {
        *Destination = R14;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r14d"))
    {
        *Destination = R14;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r14w"))
    {
        *Destination = R14;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r14b"))
    {
        *Destination = R14;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "r15"))
    {
        *Destination = R15;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "r15d"))
    {
        *Destination = R15;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "r15w"))
    {
        *Destination = R15;
        OpMaskDest = Mask16;
        return 1;
    }
    if (!strcmp(Operand, "r15b"))
    {
        *Destination = R15;
        OpMaskDest = Mask8l;
        return 1;
    }
    if (!strcmp(Operand, "rip"))
    {
        *Destination = RIP;
        OpMaskDest = Mask64;
        return 1;
    }
    if (!strcmp(Operand, "eip"))
    {
        *Destination = RIP;
        OpMaskDest = Mask32;
        return 1;
    }
    if (!strcmp(Operand, "ip"))
    {
        *Destination = RIP;
        OpMaskDest = Mask16;
        return 1;
    }
    return 0;
}
int FetchLiteral(char* Expression, long long int* Destination)
{
    if(Expression[1]== 'x')
    {
        *Destination = strtol(strchr(Operand2, 120)+1, NULL, 16);
        return 1;
    }
    else if(strtol(Expression, NULL, 16)) 
    {
        *Destination = strtol(Expression, NULL, 16);
        return 1;
    }
    return 0;
}
int ResolveAddress(long long int *Destination)
{
    char Exp[32];
    sscanf(Operand2, "%*[^[][%31[^]]", Exp);
    int Mult;
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
        FetchRegister(Op1, &Reg1);
        FetchRegister(Op2, &Reg2);
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 + Reg2 * Mult + Value;
        //printf("%lld, %lld, %d, %llx\n", Reg1, Reg2, Mult, Value);
        return 10;
    }
    if (Pointer1 && Pointer2 && Pointer4) // [ <register> "+" <register> [ "*1" | "*2" | "*4" | "*8" ]  "-" <literal> ]
    {
        sscanf(Exp, "%[^+]+%[^*]*%d-%s", Op1, Op2, &Mult, Literal);
        FetchRegister(Op1, &Reg1);
        FetchRegister(Op2, &Reg2);
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 + Reg2 * Mult - Value;
        //printf("%lld, %lld, %d, %llx\n", Reg1, Reg2, Mult, Value);
        return 11;
    }
    if (Pointer1 && Pointer2 && Pointer1 < Pointer2) // [ <register> "+" <register> [ "*1" | "*2" | "*4" | "*8" ]]
    {
        sscanf(Exp, "%[^+]+%[^*]*%d", Op1, Op2, &Mult);
        FetchRegister(Op1, &Reg1);
        FetchRegister(Op2, &Reg2);
        *Destination = Reg1 + Reg2 * Mult;
        //printf("%lld, %lld, %d\n", Reg1, Reg2, Mult);
        return 12;
    }
    if (Pointer1 && Pointer2 && Pointer1 > Pointer2) // [<register> [ "*1" | "*2" | "*4" | "*8" ]  "+" <literal> ]
    {
        sscanf(Exp, "%[^*]*%d+%s", Op1, &Mult, Literal);
        FetchRegister(Op1, &Reg1);
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 * Mult + Value;
        //printf("%lld, %lld, %d\n", Reg1, Value, Mult);
        return 13;
    }
    if (Pointer2 && Pointer4 && Pointer4 > Pointer2) // [<register> [ "*1" | "*2" | "*4" | "*8" ]  "-" <literal> ]
    {
        sscanf(Exp, "%[^*]*%d-%s", Op1, &Mult, Literal);
        FetchRegister(Op1, &Reg1);
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 * Mult - Value;
        //printf("%lld, %lld, %d\n", Reg1, Value, Mult);
        return 14;
    }
    if (Pointer1) // [<register> "+" <literal> ]
    {
        sscanf(Exp, "%[^+]+%s", Op1, Literal);
        FetchRegister(Op1, &Reg1);
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 + Value;
        //printf("%lld, %lld\n", Reg1, Value);
        return 15;
    }
    if (Pointer4) // [<register> "-" <literal> ]
    {
        sscanf(Exp, "%[^-]-%s", Op1, Literal);
        FetchRegister(Op1, &Reg1);
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 - Value;
        //printf("%lld, %lld\n", Reg1, Value);
        return 16;
    }
    if (FetchRegister(Exp, &Reg1))
    {
        //printf("%lld\n", Reg1);
        *Destination = Reg1;
        return 17;
    }
    if (FetchLiteral(Exp, &Value))
    {
        //printf("%lld\n", Value);
        *Destination = Value;
        return 18;
    }
    return 19;
}
void Init()
{
    MemData = NULL;
    MemDataUnit **LastData = malloc(sizeof(MemDataUnit));

    MemInst = NULL;
    MemInstUnit **LastInst = malloc(sizeof(MemInstUnit));
    

    int CodeSegment = 0;             //code segment indicator in RAM.txt
    char *line = malloc(Mask8l);
    FILE* RAM = fopen("RAM_DEMO.txt", "rt");
    fgets(line, Mask8l, RAM);
    RIP = strtol(strchr(line, 32), NULL, 16);
    while (!feof(RAM))
    {
        fgets(line, Mask8l, RAM);
        if(strstr(line, ".text"))
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
                MemInstUnit* tmp = malloc(sizeof(MemInstUnit));
                sscanf(line, "%llx   %s  %s  %s  %s  %s   %[^\n]", &(tmp->addr), tmp->mnemonic, tmp->operand1, tmp->operand2, tmp->operand3, tmp->machinecode, tmp->assembly);
                tmp->instlength = strlen(tmp->machinecode);
                
                if(!MemInst)
                {
                    MemInst = tmp;
                    *LastInst = tmp;
                }

                (*LastInst)->next = tmp;
                (*LastInst) = tmp;
                 
            }
            else 
            {
                MemDataUnit* tmp = malloc(sizeof(MemDataUnit));

                sscanf(line, "%llx  %hhx", &(tmp->addr), &(tmp->byte));
                tmp->next = NULL;
                
                if(!MemData)
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
        FetchRegister(Operand1, &ALUin1);
        if(!FetchRegister(Operand2, &ALUin2))
        {
            long long int Address;
            if (Operand2[0] == 'Q')
            {
                ResolveAddress(&Address);
                ReadMem(Address, 64);
            }
            else if (Operand2[0] == 'D')
            {
                ResolveAddress(&Address);
                ReadMem(Address, 32);
            }
            else if (Operand2[0] == 'W')
            {
                ResolveAddress(&Address);
                ReadMem(Address, 16);
            }
            else if (Operand2[0] == 'B')
            {
                ResolveAddress(&Address);
                ReadMem(Address, 8);
            }
            else
            {
            FetchLiteral(Operand2, &ALUin2);
            }
        }
    }
void PrintList(int m) // 1 - print MemData, 0 - print MemInst
{
    if (m == 1)
    {
        MemDataUnit* tmp = MemData;
        while (tmp)
        {
            printf(" %llX \t %X \n", tmp->addr, tmp->byte);
            tmp = tmp->next;
        }
        return;
    }
    MemInstUnit* tmp = MemInst;
    while (tmp)
    {
        printf("%llX - %s - %s - %s - %s - %s - %s - %d\n", tmp->addr, tmp->mnemonic, tmp->operand1, tmp->operand2, tmp->operand3, tmp->machinecode, tmp->assembly, tmp->instlength);
        tmp = tmp->next;
    }

}