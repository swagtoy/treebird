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
