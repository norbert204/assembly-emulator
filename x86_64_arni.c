//INCLUDES
#include "x86_64.h"

#include <stdio.h>
#include <stdlib.h>

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
                toreturn |= (long long int)(((current->byte)>>(i*8))&0xFF);
                current = current->next;
                if (current->addr != address+i) //Non-following address problem
                {
                    Run = 0; //Global error variable; stop running
                    fprintf(stderr, "Memory read error! Not following address!");
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
    fprintf(stderr, "Memory read error! Data not in memory!");    
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
