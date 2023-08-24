
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
                if (current->addr != address+(i+1)) //Non-following address problem
                {
                    Run = 0; //Global error variable; stop running
                    fprintf(stderr, "Memory read error! Not following address!\n");
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
    fprintf(stderr, "Memory read error! Data not in memory!\n");    
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
        fprintf(fp, "%c\t", (char)ReadMem(i, 1));
    }
    
    fprintf(fp, ")\t");
    fprintf(fp, "%s\n", Mnemonic);

    fclose(fp);
}

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
            InstLength = tmp->instlength;
            RIP += tmp->instlength;
            return;
        }
        tmp = tmp->next;
    }
    fprintf(stderr, "Error! Instruction not found! (0x%llx)\n", RIP);
    Run = 0;
}
void Init()
{
     RAX = 0x4198694;
     RBX = 0x4198736;
     RCX = 0x4198736;
     RDX = 0x140729291588392;	// rsp+256
     RDI = 0x1;
     RSI = 0x140729291588376;	// rsp+240
     RSP = 0x140729291588136;	// Az utolsó 3 bit biztos, hogy 0.
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
                tmp->instlength = strlen(tmp->machinecode)/2;
                if (tmp->instlength % 2 != 0) tmp->instlength += 1;
                tmp->next = NULL;
                
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
        if(Operand1[0] == '_') return;

        if(!strcmp(Mnemonic, "lea"))
        {
            ResolveAddress(&ALUin2);
            return;
        }

        if(!FetchRegister(Operand1, &ALUin1, &OpMaskDest))
        {
            long long int Address;
            if (Operand1[0] == 'Q')
            {
                ResolveAddress(&Address);
                ALUin1 = ReadMem(Address, 8);
                WriteBackSize = 8;
                OpMaskDest = Mask64; // ######
            }
            else if (Operand1[0] == 'D')
            {
                ResolveAddress(&Address);
                ALUin1 = ReadMem(Address, 4);
                WriteBackSize = 4;
                OpMaskDest = Mask32;
            }
            else if (Operand1[0] == 'W')
            {
                ResolveAddress(&Address);
                ALUin1 = ReadMem(Address, 2);
                WriteBackSize = 2;
                OpMaskDest = Mask16;
            }
            else if (Operand1[0] == 'B')
            {
                ResolveAddress(&Address);
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

        if(!FetchRegister(Operand2, &ALUin2, &OpMaskSource))
        {
            long long int Address;
            if (Operand2[0] == 'Q')
            {
                ResolveAddress(&Address);
                ALUin2 = ReadMem(Address, 8);
                OpMaskSource = Mask64;
            }
            else if (Operand2[0] == 'D')
            {
                ResolveAddress(&Address);
                ALUin2 = ReadMem(Address, 4);
                OpMaskSource = Mask32;
            }
            else if (Operand2[0] == 'W')
            {
                ResolveAddress(&Address);
                ALUin2 = ReadMem(Address, 2);
                OpMaskSource = Mask16;
            }
            else if (Operand2[0] == 'B')
            {
                ResolveAddress(&Address);
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
    ALUout = ALUout & OpMaskDest;
    long long int tmp, mask;
    if(FetchRegister(Operand1, &tmp, &mask))
    {
        *WriteBackDest = ALUout;
    }
    else
    {
        WriteMem(WriteBackAddress, WriteBackSize, ALUout);
    }
}
void add()
{
    ALUout = ALUin1 + ALUin2;

    OF = (CheckSF(ALUin1, OpMaskDest) == CheckSF(ALUin2, OpMaskSource) && CheckSF(ALUout, OpMaskDest) != CheckSF(ALUin1, OpMaskDest)) ? 1 : 0;

    CF = ALUin1 > OpMaskSource - ALUin2 ? 1 : 0;
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = ALUout ? 0 : 1;
    return;
}
void addsd()
{
    ALUout = ALUin1 + ALUin2;

    OF = (CheckSF(ALUin1, OpMaskDest) == CheckSF(ALUin2, OpMaskSource) && CheckSF(ALUout, OpMaskDest) != CheckSF(ALUin1, OpMaskDest)) ? 1 : 0;

    CF = ALUin1 > OpMaskSource - ALUin2 ? 1 : 0;
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = ALUout ? 0 : 1;
    return;
}
void and ()
{
    ALUout = ALUin1 & ALUin2;
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = ALUout ? 0 : 1;
    return;
}
void call()
{
    WriteMem(RSP, 8, RIP);
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
    ALUout = ALUin1 - ALUin2;
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = ALUout ? 0 : 1;
    CF = (ALUin1 < ALUin2) ? 1 : 0;
    OF = (CheckSF(ALUin1, OpMaskDest) != CheckSF(ALUin2, OpMaskSource) && CheckSF(ALUout, OpMaskDest) != CheckSF(ALUin1, OpMaskDest)) ? 1 : 0;
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
    ALUout = ALUin1 - 1;
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = ALUout ? 0 : 1;
    OF = (ALUin1 < 0 && ALUout >= 0) ? 1 : 0;
    return;
}
void div_()
{
    long long int Divisor = ALUin1;
    long long int RAXin = RAX;
    asm(
        "cqo\n"             // Sign-extend RAX into RDX:RAX
        "divq %[divisor]\n" // Divide RDX:RAX by divisor
        "jno div_no_of"
        : "=a"(RAX), "=d"(RDX)                // Output operands
        : "0"(RAXin), [divisor] "rm"(Divisor) // Input operands
    );
    OF = 1;
    asm("div_no_of:");
    SF = 0;
    ZF = RAX ? 0 : 1;
    // CF = undefined
    return;
}

void idiv()
{
    long long int Divisor = ALUin1;
    long long int RAXin = RAX;
    OF = 0;
    asm(
        "cqo\n"              // Sign-extend RAX into RDX:RAX
        "idivq %[divisor]\n" // Divide RDX:RAX by divisor
        "jno idiv_no_of"
        : "=a"(RAX), "=d"(RDX)                // Output operands
        : "0"(RAXin), [divisor] "rm"(Divisor) // Input operands
    );
    OF = 1;
    asm("idiv_no_of:");
    SF = (RAX & 0x8000000000000000) >> 63;
    ZF = RAX ? 0 : 1;
    // CF = undefined
    return;
}

void inc()
{
    ALUout = ALUin1 + 1;
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = ALUout ? 0 : 1;
    OF = (ALUin1 > 0 && ALUout <= 0) ? 1 : 0;
    return;
}

void imul()
{
    long long int Divisor = ALUin1;
    long long int RAXin = RAX;
    OF = 0;
    asm(
        "imulq %[input1], %[input2]\n" // Multiply input1 by input2
        "jno imul_no_of"
        : [result] "=r"(ALUout) // Output operand: result in a general-purpose register
        : [input1] "r"(ALUin1), // Input operand: input1 is variable 'a'
          [input2] "r"(ALUin2)  // Input operand: input2 is variable 'b'
    );
    OF = 1;
    asm("imul_no_of:");
    SF = CheckSF(ALUout, OpMaskDest);
    ZF = RAX ? 0 : 1;
    // CF = undefined
    return;
}
void lea()
{
    ALUout = ALUin2;
    return;
}
void leave()
{
    RSP = RBP;
    RBP = ReadMem(RSP + 8, 8);
    return;
}
void loop() //????
{
    if (RCX > 0)
    {
    }
    return;
}
void movabs()
{
    ALUout = ALUin2;
    return;
}

// ###############################//
void ret()
{
    RIP = RSP;
    RSP++;
    return;
}
void mov()
{
    ALUout = ALUin2;
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
    WriteMem(RSP, size, ALUin1);
    // printf("%llX címre pusholt elem %d bájt hosszan: %llX\n", RSP, size, ALUin1);

    RSP -= size;
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
    ALUout = ReadMem(RSP + size, size);
    // printf("%llX címről popolt elem %d bájt hosszan: %llX\n", RSP+size, size, ALUout);
    RSP += size;
    // printf("RSP: %llX\n", RSP);
    return;
}
void Execute()
{
    if(RSP > RSPinit)
          {
               Run = 0;
               fprintf(stderr, "Error! RSP (%llX) > RSPinit (%llX)\n",RSP, RSPinit);
          }
    if (!strcmp(Mnemonic, "add")) add();
    if (!strcmp(Mnemonic, "addsd")) addsd();
    if (!strcmp(Mnemonic, "and")) and();
    if (!strcmp(Mnemonic, "call")) call();
    if (!strcmp(Mnemonic, "cbw")) cbw();
    if (!strcmp(Mnemonic, "cdq")) cdqe();
    if (!strcmp(Mnemonic, "cmp")) cmp();
    if (!strcmp(Mnemonic, "cwd")) cwd();
    if (!strcmp(Mnemonic, "dec")) dec();
    if (!strcmp(Mnemonic, "div")) div_();
    if (!strcmp(Mnemonic, "idiv")) idiv();
    if (!strcmp(Mnemonic, "inc")) inc();
    if (!strcmp(Mnemonic, "imul")) imul();
    if (!strcmp(Mnemonic, "lea")) lea();
    if (!strcmp(Mnemonic, "leave")) leave();
    if (!strcmp(Mnemonic, "loop")) loop();
    if (!strcmp(Mnemonic, "mov")) mov();
    if (!strcmp(Mnemonic, "movabs")) movabs();

    //##########################
    if (!strcmp(Mnemonic, "ret")) ret();
    if (!strcmp(Mnemonic, "push")) push();
    if (!strcmp(Mnemonic, "pop")) pop();
}
void Fini()
{
    SaveState();
    puts("###--Fini--###");
}

//#####################################################################//

int CheckSF(int Value, long long int Mask)
{
    if(Mask == Mask64) return (Value & 0x8000000000000000) >> 63;
    if(Mask == Mask32) return (Value & 0x80000000) >> 31;
    if(Mask == Mask16) return (Value & 0x8000) >> 15;
    if(Mask == Mask8l) return (Value & 0x80) >> 7;
    if(Mask == Mask8h) return (Value & 0x8000) >> 15;
}
int CheckZF(int Value, long long int Mask)
{
    if(Mask == Mask64) return (Mask64 & Value)?0:1;
    if(Mask == Mask32) return (Mask32 & Value)?0:1;
    if(Mask == Mask16) return (Mask16 & Value)?0:1;
    if(Mask == Mask8l) return (Mask8l & Value)?0:1;
    if(Mask == Mask8h) return ((Mask8h & Value))?0:1;
}

int FetchRegister(char* Operand, long long int* Destination, long long int* Mask)
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
    if(!sscanf(Operand2, "%*[^[][%31[^]]", Exp)) sscanf(Operand2, "%*c%31[^]]", Exp); //lea miatt, mert ott a kifejezés egyből [- el kezdődik
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
        //printf("%lld, %lld, %d, %llx\n", Reg1, Reg2, Mult, Value);
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
        //printf("%lld, %lld, %d\n", Reg1, Reg2, Mult);
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
        //printf("%lld, %lld, %d\n", Reg1, Value, Mult);
        return 14;
    }
    if (Pointer1) // [<register> "+" <literal> ]
    {
        sscanf(Exp, "%[^+]+%s", Op1, Literal);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 + Value;
        //printf("%llx, %llx\n", Reg1, Value);
        return 15;
    }
    if (Pointer4) // [<register> "-" <literal> ]
    {
        sscanf(Exp, "%[^-]-%s", Op1, Literal);
        FetchRegister(Op1, &Reg1, &Mask);
        Reg1 &= Mask;
        FetchLiteral(Literal, &Value);
        *Destination = Reg1 - Value;
        //printf("%lld, %lld\n", Reg1, Value);
        return 16;
    }
    if (FetchRegister(Exp, &Reg1, &Mask))
    {
        //printf("%lld\n", Reg1);
        Reg1 &= Mask;
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
void PrintList(char* s) // "data" - print MemData, "inst" - print MemInst
{
    if (!strcmp("data", s))
    {
        MemDataUnit* tmp = MemData;
        while (tmp)
        {
            printf(" %llX \t %X \n", tmp->addr, tmp->byte);
            tmp = tmp->next;
        }
        return;
    }
    if (!strcmp("inst", s))
    {
        MemInstUnit* tmp = MemInst;
        while (tmp)
        {
            printf("%llX - %s - %s - %s - %s - %s - %s - %d\n", tmp->addr, tmp->mnemonic, tmp->operand1, tmp->operand2, tmp->operand3, tmp->machinecode, tmp->assembly, tmp->instlength);
            tmp = tmp->next;
        }
        return;
    }
    puts("PrintList: invalid parameter!");
    
}