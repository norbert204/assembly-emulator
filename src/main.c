#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include <stdint.h>
#include<unistd.h>
#include<fcntl.h>

#include "x86_64.h"
#include "x86_64.c"

int main()
{
      Init();
//      int count = 1;
//     while(Run)
//     {
//           SaveState();
//           InstructionFetch();
//           printf("%d --- %s\n", count, Mnemonic);
//           OperandFetch();
//           Execute();
//           WriteBack();
//           if(RSP > RSPinit)
//           {
//                Run = 0;
//                fprintf(stderr, "Error! RSP (%llX) > RSPinit (%llX)\n",RSP, RSPinit);
//                break;
//           }
//           count++;      
//     }
//     Fini();

WriteMem(0x404020, 8, 0x123456787FAABBCC);
// PrintList("data");
long long int out = ReadMem(0x404020, 8);
printf("%llX\n",out);

}
