/*
 * Treebird - Lightweight frontend for Pleroma
 *
 * Licensed under the BSD 2-Clause License
 */

#ifndef STRING_HELPERS_H
#define STRING_HELPERS_H
#include <stddef.h>

/** Returns str. If NULL, returns empty string */
#define STR_NULL_EMPTY(str) ((str) ? (str) : "")
#define MAKE_FOCUSED_IF(tab, test_tab) ((tab) == (test_tab) ? "active" : "")
#define ACTIVE_CONDITION(cond) ((cond) ? "active" : "")
#define CAT_TEXT(cat, cfg_cat) (((cat) == (cfg_cat)) ? "active" : "")

/**
 * Constructs a string based on a function
 *
 * @param func A function, with void* being the string,
 *  size_t being an index, and int* being the returned parse size
 * @param strings The array of strings
 * @param strings_len len of `strings`
 * @param ret_size Set to the full result string size, useful for large
 *  results
 * @return The result, this MUST be free'd when finished and checked for NULL.
 */
char* construct_func_strings(char* (*func)(void*, size_t, size_t*),
                             void* strings,
                             size_t strings_len,
                             size_t* ret_size);

#endif // STRING_HELPERS_H
