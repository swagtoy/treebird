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

#ifndef KEY_H
#define KEY_H
#include <string.h>
#include <stddef.h>

// Macros which make things a bit easier to read
#define keystr(key) key.type.s
#define keyint(key) key.type.i
#define keyfile(key) key.type.f
#define keypstr(key) key->type.s
#define keypint(key) key->type.i
#define keypfile(key) key->type.f

struct file_content
{
    char* content;
    size_t content_size;
    char* filetype;
    char* filename;
};

struct file_array
{
    struct file_content* content;
    size_t array_size;
};

/** Type used for each query */
struct key
{
    union u_type
    {
        int i;
        char* s;
        struct file_array f;
    } type;
    int is_set;
};

struct key_value_refs
{
    char* key;
    struct key* val;
    void (*func)(char*, struct file_content*, struct key*);
};

void key_string(char* val, struct file_content* props, struct key* arg);
void key_int(char* val, struct file_content* form, struct key* arg);
void key_files(char* val, struct file_content* form, struct key* arg);

#endif // KEY_H
