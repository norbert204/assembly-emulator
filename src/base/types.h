#pragma once

typedef enum 
{
    NO_SEGMENT = 0,
    TEXT,
    RODATA,
    DATA,
    BSS,
} segment;

typedef union
{
    long long int original_number;
    char bytes[8];
} number_as_bytes;
