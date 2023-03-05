#include "string_utils.h"
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

// Checks if the string starts with the other given string.
int str_starts_with(const char *str, const char *sub)
{
    size_t len_str = strlen(str);
    size_t len_sub = strlen(sub);

    return len_str < len_sub
        ? 0
        : strncmp(str, sub, len_sub) == 0;
}

// Removes whitespace characters from the front and the back of the given string.
void str_trim(char **str)
{
    int start;
    int end;

    int i = 0;

    size_t length = strlen(*str);

    while ((*str)[i] == ' ' || (*str)[i] == '\t')
    {
        i++;
    }

    start = i;

    i = length - 1;

    while (i >= 0 && ((*str)[i] == ' ' || (*str)[i] == '\t' || (*str)[i] == '\n'))
    {
        i--;
    }

    end = i;

    char *tmp = calloc(length, sizeof(char));

    strcpy(tmp, *str);

    memset(*str, '\0', length + 1);

    strncpy(*str, tmp + start, end - start + 1);

    free(tmp);
}

// Removes all occurences of the char specified from the given string.
void str_remove_char(char **str, const char c)
{
    char *result = calloc(sizeof(char), strlen(*str) + 1);

    size_t length = strlen(*str);

    int j = 0;
    for (int i = 0; i < strlen(*str); i++)
    {
        if ((*str)[i] != c)
        {
            result[j] = (*str)[i];
            j++;
        }
    }

    result[j] = '\0';

    memset(*str, '\0', length);

    strcpy(*str, result);

    free(result);
}

// Checks if the given string only contains the character specified.
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

// Returns the location of the first occurence of the give character in the string.
int str_index_of(const char *str, const char c)
{
    char *c_location = strchr(str, c);

    if (c_location == NULL)
    {
        return -1;
    }

    return (int)(c_location - str);
}

// Returns the location of the last occurence of the give character in the string.
int str_last_index_of(const char *str, const char c)
{
    char *c_location = strrchr(str, c);

    if (c_location == NULL)
    {
        return -1;
    }

    return (int)(c_location - str);
}
