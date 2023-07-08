#include "string_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

bool str_starts_with(const char *str, const char *sub)
{
    size_t len_str = strlen(str);
    size_t len_sub = strlen(sub);

    return len_str < len_sub
        ? 0
        : strncmp(str, sub, len_sub) == 0;
}

void str_trim(char *str, const size_t size)
{
    int start;
    int end;

    int i = 0;

    while (str[i] == ' ' || str[i] == '\t')
    {
        i++;
    }

    start = i;

    i = size - 2;

    while (i >= 0 && (str[i] == ' ' || str[i] == '\t' || str[i] == '\n' || str[i] == '\0'))
    {
        i--;
    }

    end = i;

    char tmp[size];

    strcpy(tmp, str);

    memset(str, '\0', size);

    strncpy(str, tmp + start, end - start + 1);
}

void str_remove_char(char *str, const size_t size, const char c)
{
    char result[size];

    int j = 0;
    for (int i = 0; i < strlen(str); i++)
    {
        if (str[i] != c)
        {
            result[j] = str[i];
            j++;
        }
    }

    result[j] = '\0';

    strncpy(str, result, size);
}

bool str_contains_only(const char *str, const char c)
{
    bool res = true;
    int i = 0;

    while (str[i] != '\0' && res)
    {
        if (str[i] != c)
        {
            res = false;
        }
        i++;
    }

    return res;
}

int str_index_of(const char *str, const char c)
{
    char *c_location = strchr(str, c);

    if (c_location == NULL)
    {
        return -1;
    }

    return (int)(c_location - str);
}

int str_last_index_of(const char *str, const char c)
{
    char *c_location = strrchr(str, c);

    if (c_location == NULL)
    {
        return -1;
    }

    return (int)(c_location - str);
}

int str_char_count(const char *str, const size_t size, const char c)
{
    int count = 0;

    for (int i = 0; i < size - 1; i++)
    {
        if (str[i] == '\0')
        {
            return count;
        }

        if (str[i] == c)
        {
            count++;
        }
    }

    return count;
}

bool str_empty(const char *str)
{
    if (str[0] == '\0')
    {
        return true;
    }

    return false;
}
