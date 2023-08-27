/*** x86-64 emulator system components ***/

/* Register set of the CPU's ISA */
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

/* Arithmetic and Logic Unit management */
long long int ALUin1;             // inner register for ALU first input
long long int ALUin2;             // inner register for ALU second input
long long int ALUout;             // inner register for ALU result
long long int OpMaskDest;         // mask according to the destination operand
long long int OpMaskSource;       // mask according to the source operand

/* RAM management */
typedef struct MemDataUnit{       // element of data memory linked list
   long long int addr;            // memory address
   unsigned char byte;            // data byte
   struct MemDataUnit *next;      // link for the next data list entry
} MemDataUnit;

typedef struct MemInstUnit{       // element of instruction memory linked list
   long long int addr;            // memory address
   char mnemonic[32];             // assembly mnemonic
   char operand1[32];             // simplified destination operand
   char operand2[32];             // simplified source operand
   char assembly[256];            // original text of assembly instruction
   unsigned char machinecode[32]; // bytes of the machine code
   int instlength;                // number of bytes in the machine code
   struct MemInstUnit *next;      // link for the next instruction list entry
} MemInstUnit;

MemDataUnit *MemData;             // head pointer of data memory linked list
MemInstUnit *MemInst;             // head pointer of instruction memory linked list

/* Values for the current fetch-execute loop */
long long int Address;            // current memory address
int RW_Size;                      // number of bytes in memory access
char Mnemonic[32];                // mnemonic of current instruction
char Operand1[32];                // simplified destination operand
char Operand2[32];                // simplified source operand
char Assembly[256];               // text of assembly instruction
int InstLength;                   // number of bytes in the current machine code
unsigned char MachineCode[32];    // bytes of the current machine code

/* Further accessories */
long long int RSPinit;            // initial value of RSP
int Run;                          // main loop flag (1: repeat fetch-execute loop; 0: terminate run)

/* Headers of subroutines */
void Init();
// Read RAM file to initialize RIP
// Read RAM file to initialize instruction memory linked list
// Read RAM file to initialize data memory linked list
// Initialize register set
// Opening output file

long long int ReadMem(long long int address, int size);
// Search for the given address in the data memory linked list
// If necessary compose multi-byte data from the consecutive data bytes (using little-endian byte order)
// Return with the given data

void WriteMem(long long int address, int size, long long int data);
// Search for the given address in the data memory linked list
// Save all the data bytes with the corresponding addresses into the linked list (if necessary insert new entries)
// Use little-endian byte order

void InstructionFetch();
// Read an instruction from the instruction memory list addressed by RIP
// Update: Mnemonic, Operand1, Operand2, Assembly, InstLength, MachineCode, RIP

void OperandFetch();
// Load the proper values into the ALUin1 and ALUin2 based on Operand1, Operand2
// Apply constant, register content or data memory content (using proper addressing mode)

void Execute();
// Call the required procedure for the proper operation
// Update the Run flag (based on RSP and RSPinit)

/* JUMP INSTRUCTIONS : */ void ja(), jae(), jb(), jbe(), jc(), je(), jg(), jge(), jl(), jle(), jmp(), jnc(), jne(), jnz(), jz();
/* SET INSTRUCTIONS : */ void seta(), setae(), setb(), setbe(), setc(), sete(), setg(), setge(), setl(), setle(), setnc(), setne(), setnz(), setz();
/* SHIFT AND ROTATE INSTRUCTIONS : */ void rol(), ror(), sal(), sar(), shl(), shr();

void ProcadureForAllMnemoninc();
// Based on the Mnemonic do the proper action (using ALUin1, ALUin2 and others)
// Set of procedures for mnemonics (e.g: add(), jmp() ret(), etc.)

void WriteBack();
// If necessary update the destination operand (and/or other registers) by ALUout

void SaveState();
// Save registers (and stack) into output file

void Fini();
// Free data and instruction memory linked lists
// Close files
