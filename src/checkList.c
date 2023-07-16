#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "x86_64.h"
#include "x86_64.c"

int main(){
     Init();
     //PrintList(1);
     RAX = 0x21;
     printf("%llx\n", RIP);
     InstructionFetch();
     OperandFetch();
     printf("%llx .. %llx\n",ALUin1, ALUin2);
     printf("%llx\n", RIP);
}
