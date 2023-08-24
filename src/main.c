#include <stdio.h>
#include <string.h>
#include <stdlib.h>


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
          count++;      
    }
    Fini();

}
