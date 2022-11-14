/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 3-Clause License
 */

#ifndef TREE_STRING_H
#define TREE_STRING_H
#include <stddef.h>
#include <time.h>

#define STRREPL_ALL 1

/**
 * Converts unix time into relative string format
 *
 * Longer variants for months and years to emphasize age
 *
 * Example: 5s, 5m, 5h, 5d, 5mon, 5yr
 * @param time Unix timestamp
 * @return char[16] containing time string
 */
char* reltime_to_str(time_t time);

int streql(char* cmp1, char* cmp2);
int strneql(char* cmp1, char* cmp2, size_t cmp1_n);
char* strnstr(const char* haystack, const char* needle, size_t s);
char* strrepl(char* source, char* find, char* replace, int flags);

char* sanitize_html(char* html);

#endif // TREE_STRING_H
