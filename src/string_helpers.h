/*
 * Treebird - Lightweight frontend for Pleroma
 * Copyright (C) 2022 Nekobit
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Affero General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Affero General Public License for more details.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
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
char* construct_func_strings(char* (*func)(void*, size_t, int*),
                             void* strings,
                             size_t strings_len,
                             size_t* ret_size);

#endif // STRING_HELPERS_H
