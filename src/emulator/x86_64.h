/*** x86-64 emulator system components ***/

#ifndef X86_64_H
/* Register set of the CPU's ISA */
extern long long int RAX;                // 64-bit general purpose A register
extern long long int RBX;                // 64-bit general purpose B register
extern long long int RCX;                // 64-bit general purpose C register
extern long long int RDX;                // 64-bit general purpose D register
extern long long int RSI;                // 64-bit source index register
extern long long int RDI;                // 64-bit destination index register
extern long long int RSP;                // 64-bit stack pointer register
extern long long int RBP;                // 64-bit base pointer register
extern long long int R8;                 // 64-bit general purpose 8 register
extern long long int R9;                 // 64-bit general purpose 9 register
extern long long int R10;                // 64-bit general purpose 10 register
extern long long int R11;                // 64-bit general purpose 11 register
extern long long int R12;                // 64-bit general purpose 12 register
extern long long int R13;                // 64-bit general purpose 13 register
extern long long int R14;                // 64-bit general purpose 14 register
extern long long int R15;                // 64-bit general purpose 15 register
extern long long int RIP;                // 64-bit instruction pointer register
extern int OF;                           // overflow flag bit of RFLAGS
extern int CF;                           // carry flag bit of RFLAGS
extern int SF;                           // sign flag bit of RFLAGS
extern int ZF;                           // zero flag bit of RFLAGS

/* Arithmetic and Logic Unit management */
extern long long int ALUin1;             // inner register for ALU first input
extern long long int ALUin2;             // inner register for ALU second input
extern long long int ALUout;             // inner register for ALU result
extern long long int OpMaskDest;         // mask according to the destination operand
extern long long int OpMaskSource;       // mask according to the source operand

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
   char operand3[32];             // simplified (optional) source operand
   char assembly[256];            // original text of assembly instruction
   unsigned char machinecode[32]; // bytes of the machine code
   int instlength;                // number of bytes in the machine code
   struct MemInstUnit *next;      // link for the next instruction list entry
} MemInstUnit;

extern MemDataUnit *MemData;             // head pointer of data memory linked list
extern MemInstUnit *MemInst;             // head pointer of instruction memory linked list

/* Values for the current fetch-execute loop */
extern long long int Address;            // current memory address
extern int RW_Size;                      // number of bytes in memory access
extern char Mnemonic[32];                // mnemonic of current instruction
extern char Operand1[32];                // simplified destination operand
extern char Operand2[32];                // simplified source operand
extern char Operand3[32];                // simplified (optional) source operand
extern char Assembly[256];               // text of assembly instruction
extern int InstLength;                   // number of bytes in the current machine code
extern unsigned char MachineCode[32];    // bytes of the current machine code

/* Further accessories */
extern long long int RSPinit;            // initial value of RSP
extern int Run;                          // main loop flag (1: repeat fetch-execute loop; 0: terminate run)

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
// Set the ALU masks
// Apply constant, register content or data memory content (using proper addressing mode)

void ProcadureForAllMnemoninc();
// Based on the Mnemonic do the proper action (using ALUin1, ALUin2 and others)
// Set of procedures for mnemonics (e.g: add(), jmp() ret(), etc.)

void Execute();
// Call the required procedure for the proper operation
// Update the Run flag (based on RSP and RSPinit)

void WriteBack();
// If necessary update the destination operand (and/or other registers) by ALUout

void SaveState();
// Save registers (and stack) into output file

void Fini();
// Free data and instruction memory linked lists
// Close files

//#############################################################//

#define Mask64 18446744073709551615u
#define Mask32 4294967295u
#define Mask16 65535u
#define Mask8h 65280u
#define Mask8l 255u

extern long long int *WriteBackDest;
extern long long int WriteBackAddress;
extern int WriteBackSize;

int CheckSF(int Value, long long int Mask);
int CheckZF(int Value, long long int Mask);
void PrintList(char* s);
int FetchRegister(char* Operand, long long int* Destination, long long int* Mask);
int FetchLiteral(char* Expression, long long int* Destination);
int ResolveAddress(long long int *Destination, char* Operand);

#define X86_64_H
#endif
