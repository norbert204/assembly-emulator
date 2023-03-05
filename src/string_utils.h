#pragma once

#include <string.h>
#include <stdbool.h>

int str_starts_with(const char *str, const char *sub);

void str_trim(char **str);

void str_remove_char(char **str, char c);

bool str_contains_only(const char *str, const char c);

int str_index_of(const char *str, const char c);

int str_last_index_of(const char *str, const char c);
