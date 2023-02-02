#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "x86_64.h"

void PrintList(int m) // 0 - print MemData, 1 - print MemInst
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

int main(){
    Init();
    PrintList(0);
    PrintList(1);
}
