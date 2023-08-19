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
     int count = 1;
    while(Run)
    {
          SaveState();
          InstructionFetch();
          printf("%d --- %s\n", count, Mnemonic);
          OperandFetch();
          Execute();
          WriteBack();
          if(RSP < RSPinit)
          {
               Run = 0;
               fprintf(stderr, "Error! RSP < RSPinit %llx  %llx\n",RSP, RSPinit);
               break;
          }
          count++;      
    }
    Fini();






}
