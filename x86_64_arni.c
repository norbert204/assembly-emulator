//INCLUDES
#include "x86_64.h"

#include <stdio.h>
#include <stdlib.h>

void insertAfter(struct MemDataUnit* prev_node, long long int new_data)
{
    /*1. check if the given prev_node is NULL */
    if (prev_node == NULL) {
        return;
    }
 
    /* 2. allocate new node */
    struct MemDataUnit* new_node
        = (struct MemDataUnit*)malloc(sizeof(struct MemDataUnit));
 
    /* 3. put in the data */
    new_node->byte = new_data;
 
    /* 4. Make next of new node as next of prev_node */
    new_node->next = prev_node->next;
 
    /* 5. move the next of prev_node as new_node */
    prev_node->next = new_node;
}

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
                toreturn || current->byte << i*8;
                current = current->next;
            }
            return toreturn;
        }
        else
        {
            current = current->next;
        }        
    }    
    return NULL;
}

void WriteMem(long long int address, int size, long long int data)
{
    //Data memory
    MemDataUnit * current = MemData;
    MemDataUnit * lower = NULL;
    while (current != NULL) {
        if (current->addr < address)
        {
            lower = current;
        }        
        if (current->addr == address)
        {
            for (int i = 0; i < size; i++)
            {                
                current->byte = data << i*8;
                current = current->next;
            }            
        }
        else
        {
            current = current->next;
        }
    }
    for (int i = 0; i < size; i++)
    {
        insertAfter(lower, data);
    }
    return NULL;
}
