#pragma once

#include <string.h>
#include <stdbool.h>

/* Checks if the string starts with the other given string. */
bool str_starts_with(const char *str, const char *sub);

/* Removes whitespace characters from the front and the back of the given string.
   Important! The size parameter represents the size of the array! */
void str_trim(char *str, const size_t size);

/* Removes all occurences of the char specified from the given string.
   Important! The size parameter represents the size of the array! */
void str_remove_char(char *str, const size_t size, char c);

/* Checks if the given string only contains the character specified. */
bool str_contains_only(const char *str, const char c);

/* Returns the location of the first occurence of the give character in the string. */
int str_index_of(const char *str, const char c);

/* Returns the location of the last occurence of the give character in the string. */
int str_last_index_of(const char *str, const char c);

/* Returns the count of the specified char in the string.
   Important! The size parameter represents the size of the array! */
int str_char_count(const char *str, const size_t size, const char c);

/* Checks if the given string is empty. */
bool str_empty(const char *str);
